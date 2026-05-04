// --- QUẢN LÝ KẾT NỐI ---
const SocketManager = {
    conn: null,
    // ĐỔI THÀNH PORT 81 ĐỂ KHỚP VỚI C++
    url: `ws://${window.location.hostname}:81/`, 

    init() {
        this.conn = new WebSocket(this.url);
        this.conn.onopen = () => {
            document.getElementById('connection-status').innerText = "Trực tuyến";
            document.getElementById('connection-status').style.background = "#22c55e";
        };
        this.conn.onclose = () => {
            document.getElementById('connection-status').innerText = "Ngoại tuyến";
            document.getElementById('connection-status').style.background = "#ef4444";
            setTimeout(() => this.init(), 3000);
        };
        // Gọi hàm onMessage toàn cục khi có tin nhắn
        this.conn.onmessage = (evt) => onMessage(evt);
    },

    transmit(payload) {
        if (this.conn && this.conn.readyState === WebSocket.OPEN) {
            this.conn.send(JSON.stringify(payload));
        }
    }
};

// --- QUẢN LÝ GIAO DIỆN ---
const UI = {
    gauges: {},
    relays: [],

    initNav() {
        document.querySelectorAll('.menu-item').forEach(item => {
            item.addEventListener('click', (e) => {
                document.querySelectorAll('.menu-item, .page-view').forEach(el => el.classList.remove('active'));
                item.classList.add('active');
                document.getElementById(item.dataset.target).classList.add('active');
            });
        });
    },

    initGauges() {
        this.gauges.temp = new JustGage({
            id: "gauge-temp-box", value: 0, min: 0, max: 80, title: "Độ C",decimals: 2,
            gaugeColor: "#e2e8f0", levelColors: ["#6366f1"]
        });
        this.gauges.humi = new JustGage({
            id: "gauge-humi-box", value: 0, min: 0, max: 100, title: "Phần trăm",decimals: 2,
            gaugeColor: "#e2e8f0", levelColors: ["#0ea5e9"]
        });
        // ĐÃ XÓA SETINTERVAL NGẪU NHIÊN Ở ĐÂY
    },
    
    toggleModal(id, show) {
        document.getElementById(id).style.display = show ? 'flex' : 'none';
    },

    refreshRelayList() {
        const container = document.getElementById('relay-list');
        container.innerHTML = this.relays.map(r => `
            <div class="relay-node">
                <i class="fa-solid fa-bolt" style="color: ${r.on ? '#f59e0b' : '#cbd5e1'}"></i>
                <h3>${r.name}</h3>
                <p>Cổng: GPIO ${r.pin}</p>
                <button class="switch-btn ${r.on ? 'active' : ''}" onclick="Core.flipRelay(${r.id})">
                    ${r.on ? 'ĐANG BẬT' : 'ĐANG TẮT'}
                </button>
            </div>
        `).join('');
    }
};

// --- XỬ LÝ NGHIỆP VỤ ---
const Core = {
    ledActive: false,
    neoActive: false,

    addNewRelay() {
        const name = document.getElementById('new-relay-name').value;
        const pin = document.getElementById('new-relay-gpio').value;
        if (!name || !pin) return alert("Vui lòng nhập đủ!");

        UI.relays.push({ id: Date.now(), name, pin, on: false });
        UI.refreshRelayList();
        UI.toggleModal('relayModal', false);
    },

    flipRelay(id) {
        const r = UI.relays.find(item => item.id === id);
        if (r) {
            r.on = !r.on;
            SocketManager.transmit({
                page: "device",
                value: { name: r.name, status: r.on ? "ON" : "OFF", gpio: r.pin }
            });
            UI.refreshRelayList();
        }
    },
    
    toggleLED() {
        this.ledActive = !this.ledActive;
        SocketManager.transmit({
            page: "device",
            value: { name: "System LED", status: this.ledActive ? "ON" : "OFF", gpio: 48 }
        });
    },  

    toggleNeo() {
        this.neoActive = !this.neoActive;
        SocketManager.transmit({
            page: "device",
            value: { name: "Neo Light", status: this.neoActive ? "ON" : "OFF", gpio: 45 }
        });
    }
};

// --- XỬ LÝ DỮ LIỆU TỪ ESP32 ---
function onMessage(event) {
    try {
        const data = JSON.parse(event.data);

        // 1. WiFi Status
        if (data.status === "online") alert("Kết nối WiFi thành công! IP: " + data.ip);
        else if (data.status === "error") alert("Lỗi: " + data.msg);

        // 2. Sensors & Chart
        if (data.temp !== undefined) {
            UI.gauges.temp.refresh(data.temp);
            UI.gauges.humi.refresh(data.hum);
            
            // Cập nhật text ánh sáng (nếu có id trong html)
            const lightVal = document.getElementById('light-val');
            if (lightVal) lightVal.innerText = data.light;

            // Cập nhật biểu đồ
            handleChartUpdate(data.temp, data.hum); 
        }

        // 3. LED 1 Sync
        if (data.led1 !== undefined) {
            const btn = document.getElementById('btn-led');
            const container = document.getElementById('led-container');
            const isOn = (data.led1 === "ON");
            Core.ledActive = isOn;
            
            if (isOn) container.classList.add('active-led'), btn.innerText = "Tắt Đèn";
            else container.classList.remove('active-led'), btn.innerText = "Bật Đèn";
        }

        // 4. LED 2 Sync (Neo)
        if (data.led2 !== undefined) {
            const btn = document.getElementById('btn-neo');
            const container = document.getElementById('neo-container');
            const isOn = (data.led2 === "ON");
            Core.neoActive = isOn;

            if (isOn) container.classList.add('active-led'), btn.innerText = "Tắt";
            else container.classList.remove('active-led'), btn.innerText = "Bật";
        }
    } catch (e) { console.warn("Lỗi JSON:", e); }
}

// --- BIỂU ĐỒ ---
let envChart;
const MAX_DATA_POINTS = 20;

function initChart() {
    const ctx = document.getElementById('historyChart').getContext('2d');
    envChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                { label: 'Nhiệt độ (°C)', borderColor: '#6366f1', data: [], tension: 0.4 },
                { label: 'Độ ẩm (%)', borderColor: '#0ea5e9', data: [], tension: 0.4 }
            ]
        },
        options: { responsive: true, maintainAspectRatio: false }
    });
}

function handleChartUpdate(temp, hum) {
    if (!envChart) return;
    const now = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });

    envChart.data.labels.push(now);
    envChart.data.datasets[0].data.push(temp);
    envChart.data.datasets[1].data.push(hum);

    if (envChart.data.labels.length > MAX_DATA_POINTS) {
        envChart.data.labels.shift();
        envChart.data.datasets[0].data.shift();
        envChart.data.datasets[1].data.shift();
    }
    envChart.update('none');
}

// --- KHỞI CHẠY ---
window.addEventListener('DOMContentLoaded', () => {
    UI.initNav();
    UI.initGauges();
    initChart();
    SocketManager.init();

    document.getElementById('config-form').addEventListener('submit', (e) => {
        e.preventDefault();
        SocketManager.transmit({
            page: "setting",
            value: {
                ssid: document.getElementById('inp-ssid').value,
                password: document.getElementById('inp-pass').value,
                token: document.getElementById('inp-token').value,
                server: document.getElementById('inp-host').value,
                port: document.getElementById('inp-port').value
            }
        });
        alert("Cấu hình đã được gửi!");
    });
});
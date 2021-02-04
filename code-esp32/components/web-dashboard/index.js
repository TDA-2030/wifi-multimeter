
let button_op = {
    class_name: "button",
    switch_id: "switchADC",
    bg_color: "",
    color: "",
    old_bg_color: "",
    old_color: "",
    init: function () {
        this.color = "white";
        this.bg_color = "#4CAF50";
    },
    my_hover: function () {
        //获取所有的li标签
        let list = document.getElementsByClassName(this.class_name);
        for (let i = 0; i < list.length; i++) {
            //注册鼠标进入事件
            list[i].onmouseover = function () {
                button_op.old_bg_color = this.style.backgroundColor;
                button_op.old_color = this.style.Color;
                this.style.cssText = `background-color: ${button_op.color};color: ${button_op.bg_color};border: 2px solid;`;
                console.log(this.style.cssText);
            };
            //注册鼠标离开事件
            list[i].onmouseout = function () {
                //恢复到这个标签默认的颜色
                this.style.cssText = `background-color: ${button_op.old_bg_color};color: ${button_op.old_color};`;
                console.log(this.style.cssText);
            };
        }
    },
    change_status: function (status) {

        let btn = document.getElementById(this.switch_id);
        if (status == 0) {
            btn.innerHTML = "打开";
            this.bg_color = "#4CAF50";
            btn.style.backgroundColor = "#4CAF50";
        } else {
            btn.innerHTML = "关闭";
            this.bg_color = "red";
            btn.style.backgroundColor = "red";
        }

    }
}

window.onload = function () {
    button_op.init();
    button_op.my_hover();
}


let timer_adc = 0;
let no_resp_num = 0;
let data_index=0;
let cur_datas=new Array(1)

function get_adc(btn_id) {
    let httpRequest = new XMLHttpRequest();

    httpRequest.open('GET', 'getadc', true);
    httpRequest.timeout = 600; // 超时时间，单位是毫秒

    httpRequest.onreadystatechange = function () {
        if (httpRequest.readyState == 4) {
            if (httpRequest.status == 200) {
                let data = httpRequest.responseText;
                document.getElementById('ADCdata').innerHTML = data;

                let c = document.getElementById("myCanvas");
                let ctx = c.getContext("2d");
                data_index = cur_datas.push(parseFloat(data));
                // if(data_index>800) {
                //     data_index = 0;
                //     ctx.clearRect(0,0,800,400);
                // }
                ctx.beginPath();
                ctx.strokeStyle = 'rgb(192, 80, 77)';
                ctx.moveTo(0, 400);
                if(data_index < 800) {
                    for (let i = 0; i < data_index; i++) {
                        ctx.lineTo(i, 400-cur_datas[i]);
                    }
                } else {
                    ctx.clearRect(0,0,800,400);
                    for (let i = 0; i < 800; i++) {
                        ctx.lineTo(i, 400-cur_datas[i+data_index-800]);
                    }
                }
                
                ctx.stroke();

                no_resp_num = 0;
            } else if (httpRequest.status == 0) {
                no_resp_num++;
                console.log("no_resp_num=" + no_resp_num);
                if (no_resp_num > 5) {
                    button_op.change_status(0);
                    timer_adc = window.clearInterval(timer_adc);
                    document.getElementById('status').innerHTML = "connect error";
                }

            } else {
                document.getElementById('status').innerHTML = httpRequest.status + " Error!\n" + httpRequest.responseText;

            }
        }
    };

    httpRequest.send();
}


function switcw_adc(btn_id) {
    if (!timer_adc) {
        button_op.change_status(1);
        timer_adc = self.setInterval('get_adc("' + btn_id + '")', 100);
        // document.getElementById('status').innerHTML = "ready";
    } else {
        button_op.change_status(0);
        timer_adc = window.clearInterval(timer_adc);
    }
}


function switch_mode(id, mode) {

    let httpRequest = new XMLHttpRequest();//第一步：创建需要的对象
    httpRequest.open('GET', 'switch_mode', true); //第二步：打开连接
    httpRequest.setRequestHeader('measure-mode', mode);
    httpRequest.send();//发送请求 将情头体写在send中

    httpRequest.onreadystatechange = function () {//请求后的回调接口，可将请求成功后要执行的程序写在其中
        if (httpRequest.readyState == 4 && httpRequest.status == 200) {//验证请求是否发送成功
            // document.getElementById(id).style.backgroundColor='#0ff';
            document.getElementById('indicator').innerHTML = mode;
        }
    };
}

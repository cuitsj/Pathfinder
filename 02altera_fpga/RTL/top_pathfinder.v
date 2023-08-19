module top_pathfinder(
	input   sys_clk,              //系统时钟
	input   sys_rst_n,             //系统复位
	input   bt_rx,               //蓝牙接收
    input   gd_rx,               //gd32接收

	output  bt_tx,            //蓝牙发送端
    output  gd_tx,            //GD32发送端
	output  led0
);

//ip核
wire clk_100m;                               //100MHz时钟信号
wire rst_n;                                 //所有模块的复位信号
wire locked;                                //locked信号拉高,锁相环开始稳定输出时钟 

//UART接收
parameter CLK = 28'd100_000_000;
parameter BPS = 17'd115200;
wire [7:0] btrx_data;
wire [7:0] gdrx_data;
wire btrx_done;
wire gdrx_done;

//系统复位与锁相环locked相与,作为其它模块的复位信号 
assign  rst_n = sys_rst_n & locked; 

//固定不改
//例化PLL IP核
pll_clk u_pll_clk(
    .areset             (~sys_rst_n),       //复位取反
    .inclk0             (sys_clk),          //50MHz时钟
    .c0                 (clk_100m),         //100MHz时钟
    .locked             (locked)            //locked信号拉高,锁相环开始稳定输出时钟 
);

//蓝牙接收
uart_rx #(
    .CLK(CLK),
    .BPS(BPS)) 
uart_rx_bt(
    .clk(clk_100m),
    .rst(rst_n),
    .rx_pin(bt_rx),
    .rx_data(btrx_data),
    .rx_done(btrx_done)
);

//发送给GD32
uart_tx #(
    .CLK(CLK),
    .BPS(BPS))
uart_tx_gd(
    .clk(clk_100m),
    .rst(rst_n),
    .tx_en(btrx_done),
    .tx_data(btrx_data),
    .tx_pin(gd_tx)
);

//接收GD32
uart_rx #(
    .CLK(CLK),
    .BPS(BPS)) 
uart_rx_gd(
    .clk(clk_100m),
    .rst(rst_n),
    .rx_pin(gd_rx),
    .rx_data(gdrx_data),
    .rx_done(gdrx_done)
);

//发送给蓝牙
uart_tx #(
    .CLK(CLK),
    .BPS(BPS))
uart_tx_bt(
    .clk(clk_100m),
    .rst(rst_n),
    .tx_en(gdrx_done),
    .tx_data(gdrx_data),
    .tx_pin(bt_tx)
);
    
flash_led flash_led_i(
    .clk(clk_100m),
    .rst_n(rst_n),
    .led0(led0)
);

endmodule

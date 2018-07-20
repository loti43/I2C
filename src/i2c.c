
#include "reg52.h"
#include "intrins.h"
#include "type_define.h"
// #include "i2c.h"

bit ack;
sbit SCL = P1^2;
sbit SDA = P1^1;
sbit WP  = P1^0;

uchar send_data[9] = {0x08,0x01,0x02,0x03,0x04,0x05,0x07,0x09,0xaa};
uchar receive_data[9] = {0};

uchar sla = 0xa0; // 器件地址
uchar suba = 0x00; //子器件地址
uchar numbyte = 9; // read or wright nums

void start_I2C(void);
void stop_I2C(void);
void slave_0();
void slave_1();
void check_ACK();
void write_Byte(uchar);
void delay(uint16);

uchar read_Byte(void);
bit write_Bytes(uchar , uchar , uchar *, uchar );
bit read_Bytes(uchar ,uchar , uchar *, uchar );

void start_I2C()
{
    SDA = 1;
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SDA = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 0;
    _nop_();
    _nop_();
}

void stop_I2C()
{
    SDA = 0;
    _nop_();
    _nop_();
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SDA = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void slave_0()
{
    SDA = 0;
    _nop_();
    _nop_();
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 0;
    _nop_();
    _nop_();
    _nop_();
    SDA = 1;// why should SDA been 1? cause when a slave answer ,always a 0 or  nothing.
    _nop_();
}
void slave_1()
{
    SDA = 1;
    _nop_();
    _nop_();
    _nop_();
    SCL = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCL = 0;
    _nop_();
    _nop_();
    _nop_();
}

void check_ACK()
{
    SDA = 1;
    _nop_();
    _nop_();
    _nop_();
    SCL = 1;
    ack = 0;
    _nop_();
    _nop_();
    if(SDA == 0) ack = 1;
    SCL = 0; //? why should do this?
}

void write_Byte(uchar j)
{
    uchar bit_cnt;
    for(bit_cnt=0;bit_cnt<=7;bit_cnt++)
    {
        if((j << bit_cnt)&0x80) SDA = 1;
        else SDA = 0;
        _nop_();
        SCL = 1;
        _nop_();_nop_();_nop_();_nop_();_nop_();
        SCL = 0;
        _nop_();_nop_();_nop_();_nop_();_nop_();
    }
}

uchar read_Byte()
{
    uchar retc;
    uchar bit_cnt;
    retc = 0;
    SDA = 1;
    for(bit_cnt=0;bit_cnt<=7;bit_cnt++)
    {
        _nop_();
        SCL = 1;
        _nop_();_nop_();_nop_();_nop_();_nop_();
        retc = retc << 1;
        if(SDA == 1) retc += 1;
        _nop_();_nop_();_nop_();
        SCL = 0;
        _nop_();
        _nop_();
        _nop_();
    }
    return retc;
}

bit write_Bytes(uchar sla, uchar suba, uchar *s, uchar nums_of_bytes_to_be_writed)
{
    uchar i;
    start_I2C();
    write_Byte(sla);// send device code
    check_ACK();
    if(!ack) return(0);
    write_Byte(suba); //器件单元地址
    check_ACK();
    if(!ack) return(0);
    for(i = 0 ; i< nums_of_bytes_to_be_writed;i++)
    {
        write_Byte(*s);
        check_ACK();
        if(!ack) return(0);
        s++;
    }
    stop_I2C();
    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
    return(1);
}

bit read_Bytes(uchar sla,uchar suba, uchar *s, uchar nums_of_bytes_to_be_writed)
{
    uchar i;
    start_I2C();
    write_Byte(sla);
    check_ACK();
    if(!ack) return(0);
    write_Byte(suba);
    if(!ack) {
        return 0;
    }
    delay(10000);
    start_I2C();
    write_Byte(0xa1);// why plus 1?  表示读模式
    check_ACK();
    if(!ack) return(0);
    for(i = 0; i< nums_of_bytes_to_be_writed-1;i++)
    {
        *s = read_Byte();
        slave_0();
        s++;
    }
    *s = read_Byte();
    slave_1();
    stop_I2C();
    return(1);
}

void delay(uint16 j)
{
    while(j--);
}

void main()
{
    P2 = 0;
    WP = 0;
    delay(300);
    write_Bytes(sla,suba,send_data,numbyte);
    delay(1000);
    read_Bytes(sla,suba,receive_data,numbyte);
    while(1);
}

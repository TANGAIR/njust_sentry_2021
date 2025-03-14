#ifndef _REFEREE_SYSTEM_H
#define _REFEREE_SYSTEM_H
#include "sys.h"


//底盘功率限制
#define POWER_LIMIT 100

//弹丸速度
extern float Up_Bullet_Speed;
extern float Down_Bullet_Speed;


/********************************************裁判系统相关数据结构体声明*****************************************************/


/*****************************************************数据格式**************************************************/
//帧头格式
typedef struct
{                                       //字节偏移     大小     说明
	u8 SOF;                               //   0          1      数据帧起始字节，固定值为0XA5
	u16 DataLength;                       //   1          2      数据帧内Data长度 
	u8 Seq;                               //   3          1      包序号 
	u8 CRC8;                              //   4          1      帧头CRC校验
}FrameHeader_TypeDef;


//通信协议
typedef struct
{                                       //字节偏移     大小     说明
	FrameHeader_TypeDef FrameHeader;      //   0          5      帧头协议
	u16 CmdID;                            //   5          2      命令码ID
	u16 Date[50];                         //   7          n      数据
	u16 FrameTail;                        //   7+n        2      CRC16校验
}RefereeInfo_TypeDef;

/*****************************************************比赛信息**************************************************/
//比赛状态信息0x0001
typedef __packed struct
{
 uint8_t game_type : 4;
 uint8_t game_progress : 4;     //4-7 bit：当前比赛阶段? 0：未开始比赛； ? 1：准备阶段； ? 2：自检阶段； ? 3：5s 倒计时； ? 4：对战中； ? 5：比赛结算中
 uint16_t stage_remain_time;    //当前阶段剩余时间，单位 s
 uint64_t SyncTimeStamp;        // 机器人接收到该指令的精确 Unix 时间，当机载端收到有效的 NTP 服务器授时后生效

} ext_game_state_t; 


//比赛胜负数据  0x0002
typedef __packed struct
{                                       //字节偏移     大小     说明	
  uint8_t winner;                       //   0          1      0-平局，1-红方胜，2-蓝方胜
}extGameResult_t;

/*****************************************************自身信息**************************************************/
//比赛机器人状态：0x0201。发送频率：10Hz
//1：红方英雄机器人； 2：红方工程机器人；3/4/5：红方步兵机器人； 6：红方空中机器人； 
//7：红方哨兵机器人； 8：红方飞镖机器人；9：红方雷达站；
//101：蓝方英雄机器人；102：蓝方工程机器人；103/104/105：蓝方步兵机器人；106：蓝方空中机器人；
//107：蓝方哨兵机器人；108：蓝方飞镖机器人；109：蓝方雷达站
typedef __packed struct
{                                         //字节偏移量 大小 说明
 uint8_t robot_id;                        //0 1  本机器人 ID：
 uint8_t robot_level;                     //1 1  机器人等级： 1：一级；2：二级；3：三级
 uint16_t remain_HP;                      //2 2  机器人剩余血量
 uint16_t max_HP;                         //4 2  机器人上限血量
 uint16_t shooter_id1_17mm_cooling_rate;  //6 2  机器人 1 号 17mm 枪口每秒冷却值
 uint16_t shooter_id1_17mm_cooling_limit; //8 2  机器人 1 号 17mm 枪口热量上限
 uint16_t shooter_id1_17mm_speed_limit;   //10 2 机器人 1 号 17mm 枪口上限速度 单位 m/s
 uint16_t shooter_id2_17mm_cooling_rate;  //12 2 机器人 2 号 17mm 枪口每秒冷却值
 uint16_t shooter_id2_17mm_cooling_limit; //14 2 机器人 2 号 17mm 枪口热量上限
 uint16_t shooter_id2_17mm_speed_limit;   //16 2 机器人 2 号 17mm 枪口上限速度 单位 m/s
 uint16_t shooter_id1_42mm_cooling_rate;  //18 2 机器人 42mm 枪口每秒冷却值
 uint16_t shooter_id1_42mm_cooling_limit; //20 2 机器人 42mm 枪口热量上限
 uint16_t shooter_id1_42mm_speed_limit;   //22 2 机器人 42mm 枪口上限速度 单位 m/s
 uint16_t chassis_power_limit;            //24 2 机器人底盘功率限制上限
 uint8_t mains_power_gimbal_output : 1;   //26 1 主控电源输出情况：0 bit：gimbal 口输出： 1 为有 24V 输出，0 为无 24v 输出；
 uint8_t mains_power_chassis_output : 1;  //                       1 bit：chassis 口输出：1 为有 24V 输出，0 为无 24v 输出；
 uint8_t mains_power_shooter_output : 1;  //                       2 bit：shooter 口输出：1 为有 24V 输出，0 为无 24v 输出；
} ext_game_robot_status_t;                

//敌方机器人血量
typedef struct
{
	short  Hero_1;                  //0-没有找到目标，1-找到目标，2-锁定目标	
	short  Standar_3;
	short  Standar_4;
	short  Standar_5;
	short  Engineering_2;
	short  Sentry_7;
	char   Hero_1_Invinceble_Time;
	char   Engineering_2_Invinceble_Time;
	char   Standar_3_Invinceble_Time;
	char   Standar_4_Invinceble_Time;
	char   Standar_5_Invinceble_Time;


}Enemy_HP;








//实时功率热量数据：0x0202。发送频率：50Hz
typedef __packed struct
{                                          //字节偏移量 大小 说明
 uint16_t chassis_volt;                    //0 2 底盘输出电压 单位 毫伏
 uint16_t chassis_current;                 //2 2 底盘输出电流 单位 毫安
 float chassis_power;                      //4 4 底盘输出功率 单位 W 瓦
 uint16_t chassis_power_buffer;            //8 2 底盘功率缓冲 单位 J 焦耳 备注：飞坡根据规则增加至 250J
 uint16_t shooter_id1_17mm_cooling_heat;   //10 2 1 号 17mm 枪口热量
 uint16_t shooter_id2_17mm_cooling_heat;   //12 2 2 号 17mm 枪口热量
 uint16_t shooter_id1_42mm_cooling_heat;   //14 2 42mm 枪口热量
} ext_power_heat_data_t;


//伤害状态：0x0206。发送频率：伤害发生后发送
typedef __packed struct
{                       //字节偏移量 大小 说明
 uint8_t armor_id : 4;  //0 1             bit 0-3：当血量变化类型为装甲伤害，代表装甲 ID，其中数值为 0-4 号代表机器人的五个装甲片，其他血量变化类型，该变量数值为 0。
 uint8_t hurt_type : 4; //                0x0 装甲伤害扣血；0x1 模块掉线扣血；0x2 超射速扣血；0x3 超枪口热量扣血；0x4 超底盘功率扣血；0x5 装甲撞击扣血
} ext_robot_hurt_t;


//实时射击信息：0x0207。发送频率：射击后发送 ,初始数据7
typedef __packed struct
{                        //字节偏移量 大小 说明
 uint8_t bullet_type;    //0 1 子弹类型: 1：17mm 弹丸 2：42mm 弹丸
 uint8_t shooter_id;     //1 1  发射机构 ID： 1：1 号 17mm 发射机构   2：2 号 17mm 发射机构   3：42mm 发射机构
 uint8_t bullet_freq;    //2 1 子弹射频 单位 Hz
 float bullet_speed;     //3 4 子弹射速 单位 m/s
} ext_shoot_data_t;




/*****************************************************其他机器人信息**************************************************/
//机器人血量数据：0x0003。发送频率：1Hz
typedef __packed struct
{                           //字节偏移量 大小 说明
 uint16_t red_1_robot_HP;   //0 2  红 1 英雄机器人血量，未上场以及罚下血量为 0
 uint16_t red_2_robot_HP;   //2 2  红 2 工程机器人血量
 uint16_t red_3_robot_HP;   //4 2  红 3 步兵机器人血量
 uint16_t red_4_robot_HP;   //6 2  红 4 步兵机器人血量
 uint16_t red_5_robot_HP;   //8 2  红 5 步兵机器人血量
 uint16_t red_7_robot_HP;   //10 2 红 7 哨兵机器人血量
 uint16_t red_outpost_HP;   //12 2 红方前哨战血量
 uint16_t red_base_HP;      //14 2 红方基地血量
 uint16_t blue_1_robot_HP;  //16 2 蓝 1 英雄机器人血量
 uint16_t blue_2_robot_HP;  //18 2 蓝 2 工程机器人血量
 uint16_t blue_3_robot_HP;  //20 2 蓝 3 步兵机器人血量
 uint16_t blue_4_robot_HP;  //22 2 蓝 4 步兵机器人血量
 uint16_t blue_5_robot_HP;  //24 2 蓝 5 步兵机器人血量
 uint16_t blue_7_robot_HP;  //26 2 蓝 7 哨兵机器人血量
 uint16_t blue_outpost_HP;  //28 2 蓝方前哨站血量
 uint16_t blue_base_HP;     //30 2 蓝方基地血量
} ext_game_robot_HP_t;


//飞镖发射状态：0x0004。发送频率：飞镖发射后发送，发送范围：所有机器人
typedef __packed struct
{                                //字节偏移量 大小 说明
 uint8_t dart_belong;            //0 1 发射飞镖的队伍：1：红方飞镖    2：蓝方飞镖
 uint16_t stage_remaining_time;  //1 2 发射时的剩余比赛时间，单位 s
} ext_dart_status_t;


//飞镖发射口倒计时：cmd_id (0x0105)。发送频率：1Hz 周期发送，发送范围：己方机器人
typedef __packed struct
{                              //字节偏移量 大小 说明
 uint8_t dart_remaining_time;   //0 1             15s 倒计时
} ext_dart_remaining_time_t;


/*****************************************************增益和场地信息**************************************************/
//机器人增益：0x0204。发送频率：1Hz
typedef __packed struct
{                                //字节偏移量 大小 说明
 uint8_t power_rune_buff;        //0 1              bit 0：机器人血量补血状态bit 1：枪口热量冷却加速bit 2：机器人防御加成bit 3：机器人攻击加成其他 bit 保留
}ext_buff_t;

//机器人 RFID 状态：0x0209。发送频率：1Hz，发送范围：单一机器人
typedef __packed struct
{                       //字节偏移量 大小   说明
 uint32_t rfid_status;  //0 4
																					//bit 0：基地增益点 RFID 状态；
																					//bit 1：高地增益点 RFID 状态；
																					//bit 2：能量机关激活点 RFID 状态；
																					//bit 3：飞坡增益点 RFID 状态；
																					//bit 4：前哨岗增益点 RFID 状态；
																					//bit 5：资源岛增益点 RFID 状态；
																					//bit 6：补血点增益点 RFID 状态；
																					//bit 7：工程机器人补血卡 RFID 状态；
																					//bit 8-31：保留
																					//RFID 状态不完全代表对应的增益或处罚状态，例如敌方已占领的高地增益点，不能
																					//获取对应的增益效果。
} ext_rfid_status_t; 


//场地事件数据：0x0101。发送频率：事件改变后发送
//bit 0：己方补给站 1 号补血点占领状态 1 为已占领；
//bit 1：己方补给站 2 号补血点占领状态 1 为已占领；
//bit 2：己方补给站 3 号补血点占领状态 1 为已占领；
//bit 3-5：己方能量机关状态：? bit 3 为打击点占领状态，1 为占领； ? bit 4 为小能量机关激活状态，1 为已激活； ? bit 5 为大能量机关激活状态，1 为已激活；
//bit 6：己方 R2 环形高地占领状态 1 为已占领；
//bit 7：己方 R3 梯形高地占领状态 1 为已占领；
//bit 8：己方 R4 梯形高地占领状态 1 为已占领；
//bit 9：己方基地护盾状态：? 1 为基地有虚拟护盾血量； ? 0 为基地无虚拟护盾血量；
//bit 10：己方前哨战状态：? 1 为前哨战存活； ? 0 为前哨战被击毁；
//bit 11 -31: 保留
typedef __packed struct
{                        

 uint32_t event_type;
} ext_event_data_t;




/*****************************************************其他信息**************************************************/
//机器人位置：0x0203。发送频率：10Hz
typedef __packed struct
{                                        //字节偏移     大小     说明
	float x;                               //   0          4      位置X坐标值，单位：米
	float y;                               //   4          4      位置Y坐标值，单位：米
	float z;                               //   8          4      位置Z坐标值，单位：米
	float yaw;                             //   12         4      枪口朝向角度，单位：度
}ext_game_robot_pos_t;


//交互数据接收信息：0x0301,整个 cmd_id 上行频率最大为 10Hz，请合理安排带宽。
typedef __packed struct
{                                              //字节偏移量 大小 说明 备注
 uint16_t data_cmd_id;                         //0 2 数据段的内容 ID  0x0200~0x02FF可以在以上 ID 段选取，具体 ID 含义由参赛队自定义
 uint16_t sender_ID;                           //2 2 发送者的 ID      需要校验发送者的 ID 正确性，例如红 1 发送给红 5，此项需要校验红 1
 uint16_t receiver_ID;                         //4 2 接收者的 ID      需要校验接收者的 ID 正确性，例如不能发送到敌对机器人的 ID
}ext_student_interactive_header_data_t;


//学生机器人间通信 cmd_id 0x0301，内容 ID:0x0200~0x02FF
typedef __packed struct
{                            //字节偏移量 大小 说明 备注
  uint8_t data[110];         //6 n            数据段 n需要小于 113
}robot_interactive_data_t;


//小地图交互信息,小地图交互信息标识：0x0303。发送频率：触发时发送。
typedef __packed struct
{
	float target_position_x;
	float target_position_y;
	float target_position_z;
	uint8_t commd_keyboard;
	uint16_t target_robot_ID;
} ext_robot_command_t;





/********************************************数据作用域拓展*****************************************************/
//裁判系统数据格式
extern RefereeInfo_TypeDef 				   RefereeInfor;
 
 
 
 //比赛信息
extern ext_game_state_t              Game_State; 
 //比赛结果
extern extGameResult_t     				   GameResult; 
 
 
 //机器人状态
extern ext_game_robot_status_t       GameRobotState;
 //实时功率热量
extern ext_power_heat_data_t  			 PowerHeatData; 
 //伤害状态
extern ext_robot_hurt_t      				 RobotHurt;
 //实时射击信息
extern ext_shoot_data_t      				 ShootData;



 //其他机器人血量数据
extern ext_game_robot_HP_t           GameRobotHP;
 //飞镖发射状态
extern ext_dart_status_t             DartStatus;
 //飞镖发射口倒计时
extern ext_dart_remaining_time_t     DartRemainingTime;
 
 
 
 //机器人增益
extern ext_buff_t       				     BuffMusk;
 //机器人 RFID 状态
extern ext_rfid_status_t     				 RfidDetect;
 //场地事件数据
extern ext_event_data_t              EventData;
 
 
 
 //机器人位置
extern ext_game_robot_pos_t    			 GameRobotPos;
 //学生机器人间通信
extern robot_interactive_data_t      ShowData;





//比赛射击限制
extern int Game_Allow_Shoot_Flag;
//当前可输出最大功率
extern float Power_Max; 
//串口3转换标志
extern short usart3_toggle_flag; 
//弹丸发射数目
extern int number_1;	

extern int My_Outpost_HP;

extern char Output_On;
/********************************************函数声明*****************************************************/
void JudgeSys_Init(void);
void USART3_InitConfig(void);
void JudgeSys_DMA_InitConfig(void); 

void Judge_Data_Process(void);
void RefereeInfo_Decode(uint8_t *UsartRx_Info);
void Student_Date_Upload( robot_interactive_data_t * Studentdata);
void Referee_System_Task(void *pvParameters);    


#endif










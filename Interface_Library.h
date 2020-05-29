#ifndef _INTERFACE_LIBRARY_h
#define _INTERFACE_LIBRARY_h

#define SDA_PIN 32
#define SCL_PIN 16

#define MAX_BUFFER 64
#define FLOAT_BUFFER 7
#define INT_BUFFER 6

#define slaveReader     0x01
#define slaveWriter     0x02

#define INTR0 1
#define INTR3 3

#define STATE_START               100
#define STATE_STOP                101
#define STATE_PWM                 102
#define STATE_STAP_UP             103
#define STATE_STAP_DOWN           104
#define STATE_SCHWENKSTEUERUNG	  105
#define STATE_BLOCKTACKTUNG       106
#define STATE_UF                  107
#define STATE_MEASURE  	          108
#define STATE_UF_STAP             109
#define STATE_UF_STAP_UP          110
#define STATE_UF_STAP_DOWN        111
#define STATE_START_ON            112
#define STATE_START_OFF           113            

#define OPTION_WITH_DEAD_TIME     301
#define OPTION_WITHOUT_DEAD_TIME  302

#define WAVEFORM_STOP             400
#define WAVEFORM_SUPERSINUS       402
#define WAVEFORM_CONST_PWM		    403


#define MEASURE_START             500
#define MEASURE_PULSFORM          501
#define MEASURE_STAP              502
#define MEASURE_UF                503
#define MEASURE_UF_STAP           504
#define MEASURE_STOP              505

#define DIRECTION_STOP            600
#define DIRECTION_LEFT            601
#define DIRECTION_RIGHT           602

#define STATE_PULSFORM            701
#define STATE_MOTOR               702
#define STATE_STAP                703
#define STATE_MOTOR_STAP          704
#define STATE_PARAM               705

	typedef struct ComData{
  ComData() {Instruction=0;State_Mode=0;State_Waveform=0;State_Option=0; State_Direction = 0; State_Stap = 0;State_Stop = 0;State_Start=0;State_Measure=0;State_Site=0;State_Param=0;}
  ComData( uint8_t i1,volatile uint16_t i2,uint16_t i3, volatile uint16_t i4,uint16_t i5, uint16_t i6,uint16_t i7,uint16_t i8,uint16_t i9,uint16_t i10,uint16_t i11)
  {
    Instruction =         i1;
    State_Mode =          i2;
    State_Waveform =      i3;
	  State_Option  =		    i4;
    State_Direction     = i5; 
    State_Stap          = i6;
    State_Stop  =         i7;
    State_Start =         i8;
    State_Measure  =      i9;
    State_Site    =       i10;
    State_Param    =      i11;      

  }
  uint8_t               Instruction;
  volatile uint16_t     State_Mode;
  uint16_t              State_Waveform;
  volatile uint16_t     State_Option;
  uint16_t              State_Direction;
  uint16_t              State_Stap;
  uint16_t              State_Stop;
  uint16_t              State_Start;
  uint16_t              State_Measure;
  uint16_t              State_Site;
  uint16_t              State_Param;
  
}ComData;
typedef struct ParamData{
ParamData(){f1=0.0;T1=0.0;q=0.0;D=0.0;Um_S=0.0;fp_S;D_S=0.0;Im_S=0.0;}
ParamData(double d1,double d2,double d3,double d4,double d5,double d6,double d7,double d8)
  {
	  f1 =   d1;
    T1 =   d2;
	  q  =   d3;
	  D  =   d4;
	  Um_S = d5;
    fp_S = d6;
    D_S  = d7;
    Im_S  = d8;
  }
  
  double        f1;
  double        T1;
  double        q;
  double        D;
  double        Um_S;
  double        fp_S;
  double        D_S;
  double        Im_S;
}ParamData;


typedef struct MeasureData{
MeasureData(){U=0;I =0;U_S=0;I_S=0;}
MeasureData(uint16_t m1,uint16_t m2,uint16_t m3,uint16_t m4)
  {
    U   =  m1;
    I   =  m2;
	  U_S =  m3;
    I_S =  m4;
  }
  uint16_t  U;      //Inverter Amplitude
  uint16_t  I;      //Inverter Current
  uint16_t  U_S;    //Messspannung am Spannungteiler Stap Converter
  uint16_t  I_S;    //Messstrom bei Stap Converter 
}MeasureData;


void send_Value(double data_, char* buffer,uint8_t buff_);
void send_Command(const byte buffer_, int instr_);
String readVoltage();
String readCurrent();
String readFrequenz();
String readStap_U();
String readStap_I();

void notFound(AsyncWebServerRequest *request);
void WiFiEvent(WiFiEvent_t event);
void testClient(const char * host, uint16_t port);
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length);
void onIndexRequest(AsyncWebServerRequest *request);
void onMotorRequest(AsyncWebServerRequest *request);
void onPulsformRequest(AsyncWebServerRequest *request);
void onStapRequest(AsyncWebServerRequest *request);
void onMotorStapRequest(AsyncWebServerRequest *request);
void onStopRequest(AsyncWebServerRequest *request);
void onCSSRequest(AsyncWebServerRequest *request);
void onPageNotFound(AsyncWebServerRequest *request);

//Web Interface Variable

extern float v; 
extern float c;
extern float f;
extern float v_s;
extern float i_s;
extern uint16_t sys_ADC;
extern float sens_U[];
extern byte buffer[];
extern char buffer_c[];

//String MENU_Message;

extern String q_Message;
extern String f1_Message;
extern String D_Message;
extern String f1_UF_Message;
extern String Up_Message;
extern String fp_Message;
extern String D_S_Message;
extern String Ip_S_Message;
extern String function;

#endif

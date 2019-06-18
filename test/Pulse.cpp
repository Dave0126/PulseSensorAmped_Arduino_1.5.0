/*
存在的几个问题：
1、大量数据的处理 。 1000条数据能计算出大约 20 次心跳数值，希望后期可以分批读取数据存入数组，保存每一次计算结果 。
2、下位机数据采集频率。程序设置为50Hz，即周期为20ms。 
*/
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define NUM_OF_SIGNAL 1000

int BPM;				//心率，beat per minute
int Signal[NUM_OF_SIGNAL];				//心电信号 
int IBI = 600;			//两次心跳间的时间 
bool Pulse = false;		//标记是否为心跳 
bool QS = false;		//
int rate[10];			//将过去10个心跳间隙值IBI保存在rate[]数组里 
unsigned long sampleCounter = 0;	//计时器 
unsigned long lastBeatTime = 0;	// 上次心跳时刻 
int P = 512;			//波峰
int T = 512;			//波谷
int thresh = 530;		//阈值 
int amp = 0;			//振幅 
bool firstBeat = true;	//标记是否是第一次心跳 
bool secondBeat = false;//标记是否是第二次心跳

int readFile(int Signal[]);
int pulseAlgorithm();

int main(){
	readFile(Signal);
	pulseAlgorithm();
	return 0;
} 

//从文件中读取数据 
int readFile(int Signal[]){
	int i=0,n;
    char data[NUM_OF_SIGNAL];
    FILE *fp;//定义文件指针
    if((fp=fopen("data.txt","r"))==NULL){//如果文件名不存在
		printf("cantfind the file!");//则输出没有找到文件
		}
       while(!feof(fp))//判断文件是否结束
       {  
            fscanf(fp,"%s",data);//逐个将文件中的数据放入字符串中
            Signal[i++]=atoi(data);//把字符串转变成数字（int）类型
            printf("%d\t",Signal[i-1]);
       }
       n=i;//n为数组中数据个数
              printf("\n\n共%d条数据\n\n",n);
       fclose(fp);//关闭文件
       return n;//返回n即数据个数的值
}

//计算心跳
int pulseAlgorithm(){
	for(int i=0;i<NUM_OF_SIGNAL;i++){
		sampleCounter += 20;//单位ms；50Hz 
		int N = sampleCounter - lastBeatTime;//当前相对于上一次振动波峰的时间 
	
		//波谷算法 
		if(Signal[i] < thresh && N > (IBI/5)*3){       //在小于阈值（thresh）并且 IBI的3/5处开始寻找波谷，以避免噪声干扰 
    		if (Signal[i] < T){                        // T是波谷 
      		T = Signal[i];                         //如果当前信号小于波谷（T），将该信号值赋予 T 
    		}
		}
		
		//波峰算法 
		if(Signal[i] > thresh && Signal[i] > P){          // 当前信号大于阈值（thresh）并且大于峰值（P）时 
    		P = Signal[i];                             // 把当前信号值赋予 P 
  		}
  	
  		//识别心跳 
  		if (N > 250){                                   // 为了避免高频噪声 
    		if ( (Signal[i] > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
      			Pulse = true;                               // 识别到心跳 
      			IBI = sampleCounter - lastBeatTime;         // 两个心跳间的时间，单位ms 
      			lastBeatTime = sampleCounter;

      			if(secondBeat){                        //如果 secondBeat == TRUE，这是secondBeat 
        			secondBeat = false;                  // 清除 secondBeat flag
        			for(int j=0; j<=9; j++){             // seed the running total to get a realisitic BPM at startup
          				rate[j] = IBI;
        			}	
      			}

      		if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        		firstBeat = false;                   // clear firstBeat flag
        		secondBeat = true;                   // set the second beat flag
													// IBI value is unreliable so discard it
      		}


      // keep a running total of the last 10 IBI values
      		unsigned int runningTotal = 600;                  // clear the runningTotal variable
      		for(int j=0; j<=8; j++){                // shift data in the rate array
        		rate[j] = rate[j+1];                  // and drop the oldest IBI value
        		runningTotal += rate[j]; 
				}             // add up the 9 oldest IBI values

      		rate[9] = IBI;                          // add the latest IBI to the rate array
      		runningTotal += rate[9];                // add the latest IBI to runningTotal
      		runningTotal /= 10;                     // average the last 10 IBI values
   			BPM = 60000/IBI; 			             // 心跳数值 BPM 
      		QS = true;								// set Quantified Self flag
			// QS FLAG IS NOT CLEARED INSIDE THIS ISR
      		
      		// 输出 
			printf("BPM:%d\t",BPM);
			printf("P:%d\t",P);
			printf("T:%d\t",T);
			printf("IBI:%d\n",IBI);                          
    		}
		}
	
	
	//经过一个信号峰值且以有一次心跳时
  		if (Signal[i] < thresh && Pulse == true){   // when the values are going down, the beat is over
    		Pulse = false;                         // reset the Pulse flag so we can do it again
    		amp = P - T;                           // （振幅）get amplitude of the pulse wave
    		thresh = amp/2 + T;                    // （将阈值设置为振幅的50%） set thresh at 50% of the amplitude
    		P = thresh;                            // reset these for next time
    		T = thresh;
  		}
	
	
		if (N > 2500){                          	// 当2.5秒后依然没有一次心跳时
    		thresh = 530;                          // 将阈值（thresh）设为默认值530 
    		P = 512;                               // 将波峰（P）设为默认值512 
    		T = 512;                               // 将波谷（T）设为默认值512 
    		lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    		firstBeat = true;                      // set these to avoid noise
    		secondBeat = false;                    // when we get the heartbeat back
  		}

	}
	return 0;
}

/*
���ڵļ������⣺
1���������ݵĴ��� �� 1000�������ܼ������Լ 20 ��������ֵ��ϣ�����ڿ��Է�����ȡ���ݴ������飬����ÿһ�μ����� ��
2����λ�����ݲɼ�Ƶ�ʡ���������Ϊ50Hz��������Ϊ20ms�� 
*/
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define NUM_OF_SIGNAL 1000

int BPM;				//���ʣ�beat per minute
int Signal[NUM_OF_SIGNAL];				//�ĵ��ź� 
int IBI = 600;			//�����������ʱ�� 
bool Pulse = false;		//����Ƿ�Ϊ���� 
bool QS = false;		//
int rate[10];			//����ȥ10��������϶ֵIBI������rate[]������ 
unsigned long sampleCounter = 0;	//��ʱ�� 
unsigned long lastBeatTime = 0;	// �ϴ�����ʱ�� 
int P = 512;			//����
int T = 512;			//����
int thresh = 530;		//��ֵ 
int amp = 0;			//��� 
bool firstBeat = true;	//����Ƿ��ǵ�һ������ 
bool secondBeat = false;//����Ƿ��ǵڶ�������

int readFile(int Signal[]);
int pulseAlgorithm();

int main(){
	readFile(Signal);
	pulseAlgorithm();
	return 0;
} 

//���ļ��ж�ȡ���� 
int readFile(int Signal[]){
	int i=0,n;
    char data[NUM_OF_SIGNAL];
    FILE *fp;//�����ļ�ָ��
    if((fp=fopen("data.txt","r"))==NULL){//����ļ���������
		printf("cantfind the file!");//�����û���ҵ��ļ�
		}
       while(!feof(fp))//�ж��ļ��Ƿ����
       {  
            fscanf(fp,"%s",data);//������ļ��е����ݷ����ַ�����
            Signal[i++]=atoi(data);//���ַ���ת������֣�int������
            printf("%d\t",Signal[i-1]);
       }
       n=i;//nΪ���������ݸ���
              printf("\n\n��%d������\n\n",n);
       fclose(fp);//�ر��ļ�
       return n;//����n�����ݸ�����ֵ
}

//��������
int pulseAlgorithm(){
	for(int i=0;i<NUM_OF_SIGNAL;i++){
		sampleCounter += 20;//��λms��50Hz 
		int N = sampleCounter - lastBeatTime;//��ǰ�������һ���񶯲����ʱ�� 
	
		//�����㷨 
		if(Signal[i] < thresh && N > (IBI/5)*3){       //��С����ֵ��thresh������ IBI��3/5����ʼѰ�Ҳ��ȣ��Ա����������� 
    		if (Signal[i] < T){                        // T�ǲ��� 
      		T = Signal[i];                         //�����ǰ�ź�С�ڲ��ȣ�T���������ź�ֵ���� T 
    		}
		}
		
		//�����㷨 
		if(Signal[i] > thresh && Signal[i] > P){          // ��ǰ�źŴ�����ֵ��thresh�����Ҵ��ڷ�ֵ��P��ʱ 
    		P = Signal[i];                             // �ѵ�ǰ�ź�ֵ���� P 
  		}
  	
  		//ʶ������ 
  		if (N > 250){                                   // Ϊ�˱����Ƶ���� 
    		if ( (Signal[i] > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
      			Pulse = true;                               // ʶ������ 
      			IBI = sampleCounter - lastBeatTime;         // �����������ʱ�䣬��λms 
      			lastBeatTime = sampleCounter;

      			if(secondBeat){                        //��� secondBeat == TRUE������secondBeat 
        			secondBeat = false;                  // ��� secondBeat flag
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
   			BPM = 60000/IBI; 			             // ������ֵ BPM 
      		QS = true;								// set Quantified Self flag
			// QS FLAG IS NOT CLEARED INSIDE THIS ISR
      		
      		// ��� 
			printf("BPM:%d\t",BPM);
			printf("P:%d\t",P);
			printf("T:%d\t",T);
			printf("IBI:%d\n",IBI);                          
    		}
		}
	
	
	//����һ���źŷ�ֵ������һ������ʱ
  		if (Signal[i] < thresh && Pulse == true){   // when the values are going down, the beat is over
    		Pulse = false;                         // reset the Pulse flag so we can do it again
    		amp = P - T;                           // �������get amplitude of the pulse wave
    		thresh = amp/2 + T;                    // ������ֵ����Ϊ�����50%�� set thresh at 50% of the amplitude
    		P = thresh;                            // reset these for next time
    		T = thresh;
  		}
	
	
		if (N > 2500){                          	// ��2.5�����Ȼû��һ������ʱ
    		thresh = 530;                          // ����ֵ��thresh����ΪĬ��ֵ530 
    		P = 512;                               // �����壨P����ΪĬ��ֵ512 
    		T = 512;                               // �����ȣ�T����ΪĬ��ֵ512 
    		lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    		firstBeat = true;                      // set these to avoid noise
    		secondBeat = false;                    // when we get the heartbeat back
  		}

	}
	return 0;
}

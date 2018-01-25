/**********************************************************************/
/*                    ͷ�ļ� ��SOUNDPLAY.H							  */
/*                    ��Դ������                                      */
/**********************************************************************/
/**********************************************************************
˵��:
���״洢��ʽ  unsigned char code MusicName{ ���ߣ�����������.......,0,0};
ĩβ��0��0 ��ʾ��������Ҫ��
��������λ����ɣ�
             ��λ��ʾ 1��7 ���߸�����
			 ʮλ��ʾ��������������1-������2-������3-������
			 ��λ��ʾ��������Ƿ�Ҫ��������0-������1-��������
			 
�����������λ������ɣ�
             ��λ��ʾ������ʱֵ����Ӧ��ϵ�ǣ�
				��ֵ��n����|0|1|2|3|4|5|6
				����������|1|2|4|8|16|32|64      ������ʾ=2^n��
			 ʮλ��ʾ����������Ч����0-2����0-��ͨ��1-������2-����
			 ��λ��ʾ����λ��0-�޷��㣬1-�з��㣻


�����ӳ���ĸ�ʽ
             paly�������������ţ������˶ȣ������ٶȣ���
			 |������                ��Ҫ���ŵ�����ָ�룬��β�ԣ�0��0��������
			 |���ţ�0-11��          ��ָ���������ٸ��������ࣻ
             |�����˶ȣ�1-3��   	��1�����˶ȣ� 2�������˶ȣ�3�����˶ȣ�
			 |�����ٶȣ�1-12000��   ��ֵԽ���ٶ�Խ�죻
***********************************************************************/	


/*************************************start*******************************************/
#ifndef __SOUNDPLAY_H__
#define __SOUNDPLAY_H__



#define SoundPlay


#define uint unsigned int 
#define uchar unsigned char 
#define SYSTEM_OSC    12000000   //�궨�� ����Ƶ�� 12MHZ
#define SOUND_SPACE   4/5        //�궨����ͨ��������ĳ��ȷ��ʣ�ÿ4�������

sbit BeepIO  =  P2^3;            //��������ܽ�Ϊ P2.3

uint code FreTab[12]={262,277,294,311,330,349,
369,392,415,440,466,494};    //ԭʼƵ�ʱ�
uchar code SignTab[7]={0,2,4,5,7,9,11};    //1-7Ƶ���ڱ��е�λ��
uchar code LengthTab[7]={1,2,4,8,16,32,64};
uchar Sound_Temp_TH0,Sound_Temp_TL0;    //�����Ĵ�����ֵ�ݴ�
uchar Sound_Temp_TH1,Sound_Temp_TL1;    //������ʱ����ֵ�ݴ桿
/*********************************************************************/


/*********************************************************************/
void InitialSound(void)
{
	BeepIO = 0;
	Sound_Temp_TH1 = (65536-(1/1200)*SYSTEM_OSC)/256;//����TH1Ӧװ��ĳ�ֵ
	Sound_Temp_TH1 = (65536-(1/1200)*SYSTEM_OSC)%256;//����TH1Ӧװ��ĳ�ֵ
	TH1 = Sound_Temp_TH1;
	TL1 = Sound_Temp_TH1;
	TMOD = 0X11;
	ET0  = 1;
	TR0  = 0;
	TR1  = 0;
	EA   = 1;
}	
/*********************************************************************/


/*********************************************************************/
void BeepTimer0(void) interrupt 1      //���������ж�
{
	BeepIO = !BeepIO;
	TH0    = Sound_Temp_TH0;
	TL0    = Sound_Temp_TH0;
}
/*********************************************************************/


/**************************************Play******************************/
void Play(uchar *Sound,uchar Signature,unsigned Octachord,uint Speed)
{
	uint NewFreTab[12];
	uchar i,j;
	uint Point,LDiv,LDiv0,LDiv1,LDiv2,LDiv4,CurrentFre,Temp_T,SoundLength;
	uchar Tone,Length,SL,SH,SM,SLen,XG,FD;
	for(i=0;i<12;i++)
	{
		j=i+Signature;
		if(j>11)
		{
			j=j-12;
			NewFreTab[i] = FreTab[i]*2;
		}
		else
			NewFreTab[i] = FreTab[j];
		
		if (Octachord ==1)
			NewFreTab[i]>>=2;
		else if(Octachord ==3)
			NewFreTab[i]<<=2;
		}
		SoundLength = 0;
		while(Sound[SoundLength] !=0x00)   //�����������
		{
			SoundLength+=2;
		}
		
		Point = 0;
		Tone = Sound[Point];
		Length = Sound[Point];             //������һ�������ĳ��ȣ�����10ms��
		LDiv0  = 12000/Speed;              //���һ�������ĳ��ȣ�����10ms��
		LDiv4 = LDiv0/4;                   //����ĸ������ĳ���
		LDiv4 = LDiv4-LDiv4*SOUND_SPACE;   //��ͨ���ʱ����
		TR0 = 0;
		TR1 = 1;
		while(Point < SoundLength)
		{
			SL=Tone%10;											//���������
			SM=Tone/10%10;									//������ߵ���
			SH=Tone/100;										//�����Ӧ������Ƶ��
			CurrentFre=NewFreTab[SignTab[SL-1]+SH];
			if(SL!=0)
			{
				if(SM==1)CurrentFre >>=2;     //����
				if(SM==3)CurrentFre <<=2;     //����
			  Temp_T = 65536-(50000/CurrentFre)*10/(12000000/SYSTEM_OSC); //�����������ֵ	
				Sound_Temp_TH0=Temp_T/256;
        Sound_Temp_TL0=Temp_T%256;
				TH0=Sound_Temp_TH0; 
        TL0=Sound_Temp_TL0+12;      //��12�Ƕ��ж���ʱ�Ĳ���
			}
			SLen=LengthTab[Length%10];		//����Ǽ�������
				XG=Length/10%10;						//����������ͣ�0��ͨ1����2������
				FD=Length/100;
			LDiv=LDiv0/SLen;							//���������������ĳ��ȣ����ٸ�10MS��
			if(FD==1)
				LDiv=LDiv+LDiv/2;
			if(XG!=1)
				if(XG==0)										//�����ͨ���������೤��
					if(SLen<=4)
						LDiv1=LDiv-LDiv4;
					else
						LDiv1=LDiv*SOUND_SPACE;
				else
					LDiv1= LDiv/2;            //�����������ĳ���
			else
				LDiv1=LDiv;
			if(SL==0) LDiv=0;
			  LDiv2=LDiv-LDiv1;
			if(SL!=0)
			{
				TR0=1;
				for(i=LDiv1;i>0;i--)
				{
					while(TF1==0);
					TH1 = Sound_Temp_TH1;
					TL1 = Sound_Temp_TH1;
					TF1 = 0;  
				}
			}
			if(LDiv2!=0)
			{
				TR0=0;
				BeepIO=0;
				for(i=LDiv2;i>0;i--)
				{
					while(TF1==0);
					TH1 = Sound_Temp_TH1;
					TL1 = Sound_Temp_TH1;
					TF1 = 0;
				}
			}
			Point+=2;
			Tone=Sound[Point];
			Length=Sound[Point+1];
		}
		BeepIO = 0;
}


/************************************end*****************************************************/
#endif
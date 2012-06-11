#include<reg52.h>
#include<i2c.h>
#include<lcd.h>
//����Ҫд��LCD���ַ�
uchar code enter[]="Please enter:";
uchar code error[]="ERROR!";
uchar code ulock[]="Unlocked!";
uchar code inputoldpsw[]="Old Password:";
uchar code inputnewpsw[]="New Password:";
uchar code retypeerror[]="Different!";
uchar code pswchanged[]="Password Changed!";
uchar code retypenewpsw[]="Retype Password:";
uchar code smssend[]="SMS alert!";
uchar psw_mod,psw_mark,psw_count,flag,re_count,error_count,temp;
ulong psw_default,psw_get,psw_1,psw_2,psw_3,first_psw,sec_psw,psw_tmp,num_get;
//�ε�������
void didi() 
{
	beep=0;
	delay(50);
	beep=1;
	delay(100);
	beep=0;
	delay(50);
	beep=1;
}
//����洢EEPROM
void psw_save(ulong psw){
	psw_1=psw%255;
	psw_2=psw/255%255;
	psw_3=psw/255/255;
	i2c_init();
	write_add(1,psw_1);
	delay(10);
	write_add(2,psw_2);
	delay(10);
	write_add(3,psw_3);
	delay(10);	
}
//�����ȡEEPROM
ulong  psw_read(){
	i2c_init();
	psw_1=read_add(1);
	delay(10);
	psw_2=read_add(2);
	delay(10);
	psw_3=read_add(3);
	delay(10);
	psw_get=(psw_3)*255*255+(psw_2)*255+psw_1;
	return psw_get;
}
//����״̬�洢����
void psw_mod_save(uchar psw_mark){
		i2c_init();
		write_add(20,psw_mark);
		delay(10);
}
//����״̬��ѯ����
uchar psw_mod_read(){
	i2c_init();
	psw_mod=read_add(20);
	delay(10);
	return psw_mod;
}
//��ʼ������
void init(){
		//��ȡpsw_mod
		psw_mod=psw_mod_read();
		//�ж��Ƿ�Ϊ��¼���һ��,����һ�γ�ʼ������
		if(psw_mod!=psw_mark){
			psw_mod_save(psw_mark);
			psw_save(psw_default);
		}
}
void SerialInit()//��ʼ�����򣨱���ʹ�ã������޷��շ���
{
	
	TMOD=0x20;//���ö�ʱ��������ʽΪ8λ�Զ�װ������
	TH1=0xfd;//װ���ֵ��������9600
	TL1=0xfd;
	TR1=1;//�򿪶�ʱ��
	SM0=0;//���ô���ͨѶ����ģʽ����10Ϊһ�����ͣ������ʿɱ䣬�ɶ�ʱ��1������ʿ��ƣ�
	SM1=1;//(ͬ��)�ڴ�ģʽ�£���ʱ�����һ�ξͷ���һ��λ������
	REN=1;//���н�������λ��Ҫ������sm0sm1�ٿ���������
}

//���п���������char�����飬������ֹ��/0��ֹͣ
void SerialSendChars(char *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while((!TI));//�ȴ���������źţ�TI=1������
		TI=0;
		str++;
	}
}

void sms_alert()
{
	SerialInit();//���ڳ�ʼ��
	//��γ�ʼ����֤sim300 GSMģ���ʼ��
	SerialSendChars("ati\r");//ati��ʼ��sim300ģ��
	delay(200);
	SerialSendChars("ati\r");//ati��ʼ��sim300ģ��
	delay(200);
	SerialSendChars("ati\r");//ati��ʼ��sim300ģ��
	delay(200);

	//������Ϣ��ʽ��ΪTEXT ģʽ
	SerialSendChars("AT+CMGF=1\r");
	delay(200);
	//�����ַ���ʽΪUCS2 ģʽ
	SerialSendChars("AT+CSCS=\"UCS2\"\r");
	delay(200);
	//���ö���Ϣ������ز���
	SerialSendChars("AT+CSMP=17,167,0,24\r");
	delay(200);
	//�����ֻ�����unicode���� //���ת��  15165337642
	SerialSendChars("AT+CMGS=\"00310035003100360035003300330037003600340032\"\r");
	//���Ͷ������ݱ��� //���ת��  //���ű������������Զ�����������������󳬹����Σ���ע�⣡
	SerialSendChars("77ED4FE18B6662A5FF1A5BC67801950181EA52A862A58B66FF0C5BC678018F93516595198BEF8D858FC74E096B21FF0C8BF76CE8610FFF01");
	SBUF=0x1A; //���ڷ��Ͷ���ָ�������
	while(!TI);//�ȴ���������źţ�TI=1������
	TI=0;
	delay(200);
}

//������
void main()
{
	//��ʼ����
	psw_default=123456;
	//��ʼ����״̬���(ÿ����¼�޸�Ϊ��ͬ)8bit
	psw_mark=0x23;
	//��ʼ��//д���ʼ����ͳ�ʼ����־��
	//��ʱ������� ��������λ������
	psw_count=0;
	//�����������
	error_count=0;
	//��¼�޸�����ʱ��������������
	re_count=0;
	//��������״̬��ʶ
	flag=0;
	//��ʼ��
	init();
	
	//lcd��ʼ�� ��Ļ��ʾ please enter
	lcd_init();
	for(num=0;num<13;num++)
	{
		write_data(enter[num]);
		delay(5);
	}
	//�ı���λ��
	write_com(0x80+0x40);
	//�رչ�����˸
	//write_com(0x0c);
	while(1){
		//����ɨ��
		P1=0xfe;
		temp=P1;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			delay(5);
			temp=P1;
			temp=temp&0xf0;
			if(temp!=0xf0)
			{
				switch(temp)
				{
					case 0x70: num_get=1; 
						break;	
					case 0xb0: num_get=2; 
						break;
					case 0xd0: num_get=3; 
						break;	
					case 0xe0: num_get=4; 
						break;
				}
			
			}
			
		}
		P1=0xfd;
		temp=P1;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			delay(5);
			temp=P1;
			temp=temp&0xf0;
			if(temp!=0xf0)
			{
				switch(temp)
				{
					case 0x70 : num_get=5; 
						break;	
					case 0xb0 : num_get=6; 
						break;
					case 0xd0 : num_get=7; 
						break;	
					case 0xe0 : num_get=8; 
						break;
				}
				
	
			}
		}
		P1=0xfb;
		temp=P1;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			delay(5);
			temp=P1;
			temp=temp&0xf0;
			if(temp!=0xf0)
			{
				switch(temp)
				{
					case 0x70 : num_get=9; 
						break;	
					case 0xb0 : num_get=10; 
						break;
					case 0xd0 : num_get=11; 
						break;	
					case 0xe0 : num_get=12; 
						break;
				}
	
			}
		}
		P1=0xf7;
		temp=P1;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			delay(5);
			temp=P1;
			temp=temp&0xf0;
			if(temp!=0xf0)
			{
				switch(temp)
				{
					case 0x70 : num_get=13; 
						break;	
					case 0xb0 : num_get=14; 
						break;
					case 0xd0 : num_get=15; 
						break;	
					case 0xe0 : num_get=16; 
						break;
				}
	
			}
		}
		//����ɨ�����
		//�ж϶�Ӧ����				
		if(num_get==11){

			//�ָ��������������֤״̬
			lcd_init();
			for(num=0;num<13;num++)
			{
				write_data(enter[num]);
				delay(5);
			}
			//��궨λ
			write_com(0x80+0x40);
			//num_get���
			num_get=0x00;
			//���ü�������״̬flag=0 //��֤����
			flag=0;
			//������������λ������
			psw_count=0;	
			//������ʱ����
			psw_tmp=0x00;
			
		}else if(num_get==12){

			//��ʾ�����������ʾ
			lcd_init();
			for(num=0;num<13;num++)
			{
				write_data(inputoldpsw[num]);
				delay(5);
			}
			//��궨λ
			write_com(0x80+0x40);
			//num_get���
			num_get=0x00;
			//���ü�������״̬flag=1 //�޸�����ǰ��֤����
			flag=1;
			//������������λ������
			psw_count=0;	
			//������ʱ����
			psw_tmp=0x00;

		}else if(num_get>0&&num_get<11) {
			if(num_get==10){
				num_get=0;
			}
			psw_tmp=psw_tmp*10+num_get;
			psw_count++;
			if(psw_count>=6){
				if(flag==0){
					if(psw_tmp==psw_read()){
						//ִ����֤�ɹ��Ժ�Ķ��� //��֤��ȷ����LED��һ��
						P1=0x00;
						for(num=1;num<10;num++){
							delay(1000);
						}
						P1=0xff;

						//��ʾ������ȷ�����ʾ
						lcd_init();
						for(num=0;num<13;num++)
						{
							write_data(enter[num]);
							delay(5);
						}
						//��궨λ
						write_com(0x80+0x40);
						//��֤�ɹ���ʾ
						for(num=0;num<9;num++)
						{
							write_data(ulock[num]);
							delay(5);
						}
						//��ʾUNLOCKEDʱ�� ������ز�ֹͣ��˸
						write_com(0x0C);
						//��ʱ
						for(num=0;num<100;num++){
							delay(100);
						}
						//���ص���������״̬
						lcd_init();
						for(num=0;num<13;num++)
						{
							write_data(enter[num]);
							delay(5);
						}
						//��궨λ
						write_com(0x80+0x40);
						//���ù����˸
						write_com(0x0f);
						//���ô���ͳ�ƴ���
						error_count=0;
						//������������λ������
						psw_count=0;	
						//������ʱ����
						psw_tmp=0x00;
					}else{
							//��ʾ���������������ʾ
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(enter[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							//������ʾerror
							for(num=0;num<6;num++)
							{
								write_data(error[num]);
								delay(5);
							}
							//��ʾerrorʱ�� ������ز�ֹͣ��˸
							write_com(0x0C);
							//��ʱ2s��ʾ����ת
							for(num=0;num<10;num++){
								delay(100);
								didi();
								didi();
							}
							//���ص���������״̬
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(enter[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							//���ù����˸
							write_com(0x0f);
							error_count++;
	
						}
						delay(1);
						if(error_count>=3){
							//�������κ�����ش��󾯸�
							//���ű���
							//��ʾ���ű���״̬
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(enter[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							for(num=0;num<10;num++)
							{
								write_data(smssend[num]);
								delay(5);
							}
							//��겻��˸����ʾ
							write_com(0x0C);
							//LED��������������
							P1=0x00;
							for(num=1;num<50;num++){
								delay(100);
								didi();
								didi();
							}
							//smsalert
							sms_alert();
							//��ʱ1s��ʾ����ת
							for(num=1;num<50;num++){
								delay(100);
							}
							//���ص���������
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(enter[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							//���ù����˸
							write_com(0x0f);
							//����������
							error_count=0;
						}
						
					}else if(flag==1){
						//flagΪ1�����޸�����ʱ�޸�����
						if(psw_tmp==psw_read()){
							//��ʾ������ȷ�����ʾ
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(inputnewpsw[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							//���洦��ڶ�������
						  	flag=3;
							//���ô���ͳ�ƴ���
							error_count=0;
							//������������λ������
							psw_count=0;	
							//������ʱ����
							psw_tmp=0x00;
							

						}else{
							//�޸�������֤ʧ�ܵ�ʱ��
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(inputoldpsw[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							//������ʾerror
							for(num=0;num<6;num++)
							{
								write_data(error[num]);
								delay(5);
							}
							//��ʾerrorʱ�� ������ز�ֹͣ��˸
							write_com(0x0C);
							//LED��������������
							P1=0x00;
							//��ʱ2s��ʾ����ת
							for(num=0;num<10;num++){
								delay(100);
								didi();
								didi();
							}
							//���ص���������״̬
							lcd_init();
							for(num=0;num<13;num++)
							{
								write_data(inputoldpsw[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							//���ù����˸
							write_com(0x0f);
							error_count++;
							delay(1);
							if(error_count>=3){
								//�������κ�����ش��󾯸�
								//���ű���
								//��ʾ���ű���״̬
								lcd_init();
								for(num=0;num<13;num++)
								{
									write_data(inputoldpsw[num]);
									delay(5);
								}
								//��궨λ
								write_com(0x80+0x40);
								//sms_alert
								for(num=0;num<10;num++)
								{
									write_data(smssend[num]);
									delay(5);
								}
								//��겻��˸����ʾ
								write_com(0x0C);
								for(num=0;num<50;num++){
									delay(100);
									didi();
									didi();
								}
								//smsalert
								sms_alert();
								//��ʱ1s��ʾ����ת
								for(num=1;num<50;num++){
									delay(100);
								}
								//���ص���������
								lcd_init();
								for(num=0;num<13;num++)
								{
									write_data(inputoldpsw[num]);
									delay(5);
								}
								//��궨λ
								write_com(0x80+0x40);
								//���ù����˸
								write_com(0x0f);
								//����������
								error_count=0;
							}
						}
	
					}else if(flag==3){ //��6�� �������޸����������� ���洦�������޸���������
						if(re_count==0){
							//��һ�ν���ʱ��ֵ��first_psw,����־λre_count=1��
							first_psw = psw_tmp;//		first_psw,sec_psw	
							re_count=1;
							//�����ڶ�������ȷ�ϵĽ���
							lcd_init();
							for(num=0;num<16;num++)
							{
								write_data(retypenewpsw[num]);
								delay(5);
							}
							//��궨λ
							write_com(0x80+0x40);
							
						}else{
							sec_psw = psw_tmp;
							if(first_psw==sec_psw){
								//�ȶ������Ƿ�һ��//���д洢
								psw_save(sec_psw);
								//��ʾ�洢�ɹ�����Ϣ
								lcd_init();
								for(num=0;num<16;num++)
								{
									write_data(retypenewpsw[num]);
									delay(5);
								}
								//��궨λ
								write_com(0x80+0x40);
								for(num=0;num<17;num++)
								{
									write_data(pswchanged[num]);
									delay(5);
								}
								//������ز�ֹͣ��˸
								write_com(0x0C);
								//��ʱ����ʾ�����޸ĳɹ�
								for(num=0;num<100;num++){
									delay(100);
								}
								
								//�ָ��������������֤״̬
								lcd_init();
								for(num=0;num<13;num++)
								{
									write_data(enter[num]);
									delay(5);
								}
								//��궨λ
								write_com(0x80+0x40);
								//num_get���
								num_get=0x00;
								//���ü�������״̬flag=0 //��֤����
								flag=0;
								//������������λ������
								psw_count=0;	
								//������ʱ����
								psw_tmp=0x00;

							}else{
								//��ʾ���벻һ�µĴ�����Ϣ
								lcd_init();
								for(num=0;num<10;num++)
								{
									write_data(retypeerror[num]);
									delay(5);
								}
								//��ʱ����ʾ������Ϣ
								for(num=0;num<10;num++){
									delay(100);
								}
								//��ת����������
								lcd_init();
								for(num=0;num<13;num++)
								{
									write_data(inputnewpsw[num]);
									delay(5);
								}
								//��궨λ
								write_com(0x80+0x40);
								//���ñ�־  �´������ٴν����ظ������޸�����
								flag=3;
								first_psw=0;
								sec_psw=0;
								re_count=0;

							}
						}
						
					}
				//������������λ������
				psw_count=0;	
				//������ʱ����
				psw_tmp=0x00;
				}else{
					//������벻��6�Σ�����������ʾ*
					write_data('*');
				}
				//��ռ�ֵ
				num_get=0x00;
			}
		//����쳣������ֵ
		num_get=0x00;
	};

}



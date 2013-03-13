//#include "mbase64.h"
#include "hyTypes.h"

///////////////////////////////////////// base64���� //////////////////////////////////////////////
const char base64key[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

/*
	data:��������ִ�
	data_len:�ִ�����
	encode:�������ִ�
	encodelen:in:encode buf�ĳ���  out:ʵ�ʱ����ĳ���   ʧ��ʱ���ر������賤��
	
	����:0:�ɹ�  -1:ʧ��
*/
int mbase64_encode(const char *data, int data_len, char *encode, int *encodelen)
{
	int		prepare = 0;
	int		ret_len;
	int		temp = 0;
	char	*f = NULL;
	int		tmp = 0;
	char	changed[4];
	int		i = 0;
	
	ret_len = data_len / 3;
	temp = data_len % 3;
	if (temp > 0)
	{
		ret_len += 1;
	}
	ret_len = ret_len*4 + 1;
	
	if(*encodelen < ret_len)//�ڴ治��
	{
		*encodelen = ret_len;
		return -1;
	}
	
	memset(encode, 0, *encodelen);
	
	f = encode;
	ret_len = 0;
	while (tmp < data_len)
	{
		temp = 0;
		prepare = 0;
		memset(changed, 0, 4);
		while (temp < 3)
		{
			if (tmp >= data_len)
			{
				break;
			}
			//if(data[tmp] != 0x0d && data[tmp] != 0x0a)
			{
				prepare = ((prepare << 8) | (data[tmp] & 0xFF));
				temp++;
			}
			tmp++;
		}
		prepare = (prepare<<((3-temp)*8));

		for (i = 0; i < 4 ;i++ )
		{
			if (temp < i)
			{
				changed[i] = 0x40;
			}
			else
			{
				changed[i] = (prepare>>((3-i)*6)) & 0x3F;
			}
			*f = base64key[changed[i]];
			f++;
			ret_len++;
		}
	}
	*f = 0;
	*encodelen = ret_len;//strlen(encode);

	return 0;
}

static char find_pos(char ch) 
{
	char *ptr = (char*)strrchr(base64key, ch);//the last position (the only) in base64key[]
	
	return (ptr - base64key);
}

/*
	data:��������ִ�
	data_len:�ִ�����
	decode:�������ִ�
	decodelen:in:decode buf�ĳ���  out:ʵ�ʽ����ĳ���   ʧ��ʱ���ؽ������賤��
	
	����:0:�ɹ�  -1:ʧ��
*/
int mbase64_decode(const char *data, int data_len, char *decode, int *decodelen)
{
	int		ret_len = (data_len / 4) * 3;
	int		equal_count = 0;
	char	*f = NULL;
	int		tmp = 0;
	int		temp = 0;
	char	need[3],ch;
	int		prepare = 0;
	int		i = 0;

	if(*decodelen < ret_len)//�ڴ治��
	{
		*decodelen = ret_len;
		return -1;
	}
	
	memset(decode, 0, *decodelen);
	
	if (*(data + data_len - 1) == '=')
	{
		equal_count += 1;
	}
	if (*(data + data_len - 2) == '=')
	{
		equal_count += 1;
	}
	if (*(data + data_len - 3) == '=')
	{//seems impossible
		equal_count += 1;
	}
	
	switch (equal_count)
	{
	case 0:
		ret_len += 4;//3 + 1 [1 for NULL]
		break;
	case 1:
		ret_len += 4;//Ceil((6*3)/8)+1
		break;
	case 2:
		ret_len += 3;//Ceil((6*2)/8)+1
		break;
	case 3:
		ret_len += 2;//Ceil((6*1)/8)+1
		break;
	default:
		break;
	}
	
	f = decode;
	ret_len = 0;
	while (tmp < (data_len - equal_count))
	{
		temp = 0;
		prepare = 0;
		memset(need, 0, 4);
		while (temp < 4)
		{
			if (tmp >= (data_len - equal_count))
			{
				break;
			}
			if(data[tmp] != 0x0d && data[tmp] != 0x0a)//base64���������ݲ����ܳ���0D  0A
			{
				ch = find_pos(data[tmp]);
				prepare = (prepare << 6) | ch;
				temp++;
			}
			else
			{

			}
			tmp++;
		}
		prepare = prepare << ((4-temp) * 6);
		for (i=0; i<3 ;i++ )
		{
			if (i == temp)
			{
				break;
			}
			*f = (char)((prepare>>((2-i)*8)) & 0xFF);
			f++;
			ret_len++;
		}
	}
	*f = 0;
	*decodelen = ret_len;
	
	return 0;
}


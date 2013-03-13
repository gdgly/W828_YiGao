#include "hyTypes.h"
#include "touchKey.h"
#include "keyCodeEvent.h"
#include "grap_view.h"

/*
 * ��Ҫ����ͨ��������ģʽ������е����������ֻ�ж̰�����������û�г���
*/

//����״̬����
#define TOUCH_STATE_IDEL   (0)
#define TOUCH_STATE_TOUCH  (1)

//��Ч��Y��������
#define TOUCH_EFFECT_Y   (410)

//��������֮�����Ч���
#define TOUCH_KEY_INTER_X  (80)

//�������������Ӧ������Ӧ
static const U8 gu16TouchXTab[]=
{
    Keyb_Code_F2,     
    Keyb_Code_OK,
    Keyb_Code_Esc,

};

//����״̬��¼
typedef struct _tTOUCH_FSM
{
    U8 state;   //״̬
    U8 curKey;  //��ǰ�İ���
}tTOUCH_FSM;

static tTOUCH_FSM  gtTouchFsm;

/*---------------------------------------------------------------------------
 ������tk_check_point_key
 ���ܣ����ڼ�¼�������ֵ���ж��Ƿ�Ϊ������
 ������������  (x,y)
 ���أ�-1 ���ǰ���  ����Ϊ����ֵ
* -----------------------------------------------------------------------------*/
int tk_check_point_key(int x, int y, int state)
{
    int i;
    int stateFsm;
    int key;
    int index;
    int rv = -1;
    int temp;
    
    //��Ҫ�жϺ�������ȷ��x,yλ��
    //if(1 == LcdModule_Get_ShowType())
    //{
    //    temp = x;
    //    x = 239-y;
    //    y = temp;
    //}
    
    //�ж�Yֵ�Ƿ�Ϸ�
    if (y < TOUCH_EFFECT_Y)
    {
        gtTouchFsm.state = TOUCH_STATE_IDEL;
        
        return rv;
    }
    
     
    index = x/TOUCH_KEY_INTER_X;
    index = index > 2? 2: index;
    
    key = gu16TouchXTab[index];
    
    stateFsm = gtTouchFsm.state;
    
    switch(stateFsm)
    {
        case TOUCH_STATE_IDEL:
            if (VIEW_PULL_START != state)
            {
                return rv;
            }
            gtTouchFsm.state = TOUCH_STATE_TOUCH;
            gtTouchFsm.curKey = key;
            break;
            
        case TOUCH_STATE_TOUCH:
            if (gtTouchFsm.curKey != key)
            {
                gtTouchFsm.state = TOUCH_STATE_IDEL; 
            }
            
            //��������̧�𣬿��Է����ð�����Ϣ
            if (VIEW_PULL_END == state)
            {
                 gtTouchFsm.state = TOUCH_STATE_IDEL;
                 
                 return key;
            }
            break;
            
    }
    
    return rv;
}
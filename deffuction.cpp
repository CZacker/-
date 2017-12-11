#include <iostream.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <string>
#include <cmath>
using namespace std;

const char NUM[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' ,'x'};
const char OPERATOR[] = { '+', '-', '*', '/' };
double precision=0.1;//精细度

int n;//函数个数（1~3）
string expsp;

//===============================================================
//是否有刻度
bool scale=true;
//单位长度
double per=1;
//滚轴精密度
double wh=0.05;
//宽高：
int width=1000;
int height=600;
//原点：
int xx=100;
int yy=100;
//作用域：
double left=-100;
double right=130;
//缩放比（sx为横向拉伸倍数，sy为纵向拉伸倍数）：
double sx=1,sy=1;

float x;

struct Unit {
    Unit(
         int p,
         char o,
         string s,
         double num,
         int t,
         Unit *pre = NULL,
         Unit *n = NULL) :
    priority(p), operate(o), sourceCode(s), number(num), type(t), pre(pre), next(n){}
    int priority; 
    char operate; 
    string sourceCode; 
    double number; 
    int type;     Unit *pre; //Previous unit
    Unit *next; //Next unit
};

class Node{   //define a class
public:
    Node(int p,
         char o,
         int e = 1,
         double num = 0,
         Node *ph = NULL,
         Node *pl = NULL,
         Node *pr = NULL
         ) :
    priority(o), operate(o), expression(e), number(num), head(ph), left(pl), right(pr){}
    Node *head;//The head of the tree
    Node *left;//Left branch
    Node *right;//Right branch
    double getValue();
    char getOperator() const {
        return operate;
    }
    int getPriority() const {
        return priority;
    }
    int isExpression() const {
        return expression;
    }
    
private:
    int priority;
    char operate;
    int expression;
    double number;
};
double Node::getValue() {   //define a function in class 
    if (isExpression())
    {
        
        double lValue, rValue;
        lValue = left->getValue();
        rValue = right->getValue();
        if (lValue==72) {
            lValue=x;
        }
        if (rValue==72) {
            rValue=x;
        }
        expression = 0;
        char op = getOperator();
        switch (op)
        {
            case '+':
                return lValue + rValue;
            case '-':
                return lValue - rValue;
            case '*':
                return lValue * rValue;
            case '/':
                if (rValue==0)
                {
                    cout << "[Error] Can't divide a number by zero!" << endl;
                }
                else
                    return lValue / rValue;
            default:
                return 0;
        }
    }
    else
    {
        return number;
    }
}

bool isDigit(char c) {
    for (int i = 0; i < sizeof(NUM); i++) {
        if (c==NUM[i])
        {
            return true;
        }
    }
    return false;
}
bool isOperator(char c) {
    for (int i = 0; i < sizeof(OPERATOR); i++)
    {
        if (c==OPERATOR[i])
        {
            return true;
        }
    }
    return false;
}

Unit * Analyze(string exp) {
    int priority = 0;
    int state = -1;
    Unit *head = NULL, *p = NULL;
    int i = 0, expLen=0;
    expLen = exp.size();
    for ( i = 0; i < expLen; i++)
    {
        char c = exp.at(i);
        if (c=='=')
        {
            if (i!=expLen-1)
            {
                cout << "'=' is not at the end of the expression!" << endl;
            }
            break;
        }
        if (isspace(c))
        {
            i++;
        }
        else if (c=='(')
        {
            priority += 3;
            state = 0;
        }
        else if (c==')')
        {
            priority -= 3;
            state = 0;
        }
        else if (isOperator(c))
        {
            if (head == NULL)
            {
                cout << "Operator can't at the beginning of the expression!" << endl;
            }
            Unit *temp = p; //Current Unit
            int addPri;
            if (c == '+' || c == '-')
            {
                addPri = 1;
            }
            else if (c == '*' || c == '/')
            {
                addPri = 2;
            }
            p->next = new Unit(priority + addPri, c, " ", 0, 0);
            p = p->next;
            p->pre = temp;
        }
        else
        {
            string num = "";
            while (i<expLen && (c=exp.at(i), isDigit(c)))
            {
                num += c;
                i++;
            }
            i--;
            if (head==NULL)
            {
                p = new Unit(priority, ' ', num, 0, 1);
                head = p;
            }
            else
            {
                Unit *temp = p;
                p->next = new Unit(priority, ' ', num, 0, 1);
                p = p->next;
                p->pre = temp;
            }
        }
    }
    return head;
}

Unit * Calculate(Unit *head) {
    Unit *p = head;
    while (p!=NULL)
    {
        if (p->type == 1) {
            string temp = p->sourceCode;
            double lValue = 0;
            int lPoint = 0;
            int i = 0;
            char ch;
            while (i<temp.size() && (ch=temp.at(i), isDigit(ch)))
            {
                if (ch == '.') {
                    lPoint++;
                    i++;
                    continue;
                }
                if (!lPoint)
                {
                    lValue *= 10;
                }
                lValue += (ch - '0') * pow(10, -lPoint);
                i++;
                if (lPoint)
                {
                    lPoint++;
                }
            }
            p->number = lValue;
        }
        p = p->next;
    }
    return head;
}

Node * Tree(Unit *head) {
    Node * root = NULL, *pRoot = NULL, *pBranch = NULL;
    Unit * p = head;
    int currentPrio=0; //Current Priority
    bool hadop = false;
    while (p!=NULL)
    {
        if (p->type == 0){ //If the current Unit is an operator
            hadop = true;
            if (root==NULL)
            {
                pRoot = new Node(p->priority, p->operate, 1);
                root = pRoot;
                pBranch = root;
                currentPrio = p->priority;
                pRoot->left = new Node(0, ' ', 0, p->pre->number);
                pRoot->right = new Node(0, ' ', 0, p->next->number);
            }
            else
            {
                if (p->priority<=currentPrio)
                {
                    pRoot = new Node(p->priority, p->operate, 1);
                    pRoot->left = root;
                    pRoot->right = new Node(0, ' ', 0, p->next->number);
                    root = pRoot;
                    pBranch = pRoot;
                }
                else
                {
                    Node *temp;
                    temp = new Node(p->priority, p->operate, 1);
                    
                    pBranch->right = temp;
                    temp->head = pBranch;
                    
                    pBranch = pBranch->right;
                    pBranch->left = new Node(0, ' ', 0, p->pre->number);
                    pBranch->right = new Node(0, ' ', 0, p->next->number);
                }
                currentPrio = p->priority;
            }
        }
        p = p->next;
    }
    if (!hadop)
    {
        root = new Node(0, ' ', 0, head->number);
    }
    return root;
}
float exps(float x1,string exp)
{
    x=x1;
    float resu;
    
    //string &assign(const char *s1);
    //if (! getline(cin,exp).eof())
    {
        if (exp == "x")
            return x1;   //code for debug
        Unit * h = Analyze(exp);
        h = Calculate(h);
        Node * root = Tree(h);
        resu=root->getValue();
    }
    return resu;
}
//*********************************************************************************************************************画图部分
void cls()
{
    clearrectangle(0,0,width,height);
    setfillstyle(WHITE);
    bar(0,0,width,height);
}
//画坐标轴
void zbz(int color)
{
    setcolor(color);
    line(xx,0,xx,height);
    line(0,height-yy,width,height-yy);
    if(scale)
    {
        setcolor(RGB(6,189,200));
        double temp1,temp2,n;
        temp1=temp2=xx;
        for(n=0;n<360;n++)
        {
            temp1=xx+n*fabs(sx)*per;
            temp2=xx-n*fabs(sx)*per;
            line((int)temp1,height-yy,(int)temp1,height-yy-3);
            line((int)temp2,height-yy,(int)temp2,height-yy-3);
        }
        temp1=temp2=yy;
        for(n=0;n<250;n++)
        {
            temp1=yy+n*fabs(sy)*per;
            temp2=yy-n*fabs(sy)*per;
            line(xx,height-(int)temp1,xx+3,height-(int)temp1);
            line(xx,height-(int)temp2,xx+3,height-(int)temp2);
        }
    }
}
//画坐标轴上的点
void dian(double x,double y)
{
    int rx=xx+(int)(x*sx);
    int ry=height-yy-(int)(y*sy);
    putpixel(rx,ry,BLACK);
    moveto(rx,ry);
}
void xian(double x,double y)//从当前点向目标点画线
{
    int rx=xx+(int)(x*sx);
    int ry=height-yy-(int)(y*sy);
    lineto(rx,ry);
    moveto(rx,ry);
}
//设置分辨率
void setsize(int w,int h)
{
    width=w;height=h;
}
//设置坐标原点
void setyuan(int yuanx,int yuany)
{
    xx=yuanx;yy=yuany;
}
//设置缩放比
void setsuo(double a,double b)
{
    sx=a;sy=b;
}
void paint();//该函数具体定义于主文件,和fromto函数的两个参数均指左右边界
void catchcase()
{
    MOUSEMSG m; 
    setbkcolor(RGB(0,0,0));
    setbkmode(TRANSPARENT);
    RECT r = {0, 0, 480,300};
    drawtext("请点击鼠标左键以确定坐标原点\n\n滚动滚轮调整整体缩放比,同时按下ctrl调整纵向缩放比\n\n按下shift调整横向缩放比,右键返回,中键截屏",&r,DT_WORDBREAK);
    while(true)
    {
        m = GetMouseMsg();
        switch(m.uMsg)
        {
            case WM_LBUTTONDOWN:// 如果点左键
                setyuan(m.x,height-m.y);
                cls();
                paint();
                break;
            case WM_RBUTTONUP:// 按鼠标右键退出程序
                return;
            case WM_MOUSEWHEEL://滚轴改变缩放比
                if(m.mkCtrl)
                {
                    sy+=wh*m.wheel;
                }
                else if(m.mkShift)
                {
                    sx+=wh*m.wheel;
                }
                else
                {
                    sy+=wh*m.wheel;
                    sx+=wh*m.wheel;
                }
                cls();
                paint();
                break;
            case WM_MBUTTONDOWN://中键按下截图
                system("mkdir pictures");
                saveimage(".\\pictures\\捕获.bmp");
                break;
        }
    }
}

void paint()
{
    zbz(RGB(6,200,47));
    double y,x;
    //setfillcolor(BLACK);
    for (x = -200; x < 200; x=x+0.1)
    {
        if(x==-12||x==12)x++;
        y=exps(x,expsp);/* code */
        //cout << x << "The result is: " << y << endl;
        if(x==-200)dian(x,y);
        else
        xian(x,y);
    }
    
    //cout << "The result is: " << y << endl;
}
//==================================================================================
int main() {
    float result1=0;
	char s[20]; 
	InputBox(s,20,"请输入需要绘制的函数");
	string &assign(const char *s);
	expsp=s;
    initgraph(width,height);
    catchcase();
    closegraph();
    return 0;
    //float result1=0,xx=2;
    //cout << "Input your expression to calculate:" << endl;
    //expsp="x*x";
    //result1=exps(xx,expsp);
    //cout << "The result is: " << result1 << endl;
    //return 0;
}

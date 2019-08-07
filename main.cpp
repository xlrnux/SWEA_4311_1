#include<iostream>
#include<stdio.h>
using namespace std;

#define INF 12345
#define SZ 1000000
// 문제풀면서 썼던 메모는 소스 최하단에 적어둠
// 다시 생각해보니 굳이 digits[1] 의 정렬이 필요없어서 정렬삭제
template<typename T>
class vector{
private:
    int _size;
    T DATA[SZ]; // 어차피 메모리는 남아 도니까 안전빵 100만
public:
    vector() : _size(0){}
    ~vector(){}
    void clear(){
        _size = 0;
    }
    void push_back(const T& val){
        DATA[_size++] = val;
    }
    void pop_back(){
        _size--;
    }

    T& operator [] (int pos){
        return DATA[pos];
    }

    T operator [] (int pos) const{
        return DATA[pos];
    }

    int size() const {
        return _size;
    }
};

int N,O,M;
// useable_op 사용가능한 연산자
bool useable_op[5] = {true, false,false,false,false}; // 선행가지치기때 필요
int pushMemo[1000] = {0,}; // 해당 숫자에 도달 할 때까지 필요 숫자 메모
vector<int> digits[4]; // 조합가능한 숫자 목록. digits[자릿수=누른횟수]
vector<int> oparts; // 사용 가능한 연산자 목록
int target = 0;
bool suc = false;
inline int oper(const int& num1,const int& num2, const int& op){
    //const char conv[5] = {'=', '+', '-', '*', '/'};
    int ret = 0;
    if(op == 1){
        ret = num1 + num2;
    }else if(op == 2){
        ret = num1 - num2;
    }else if(op == 3){
        ret = num1 * num2;
    }else if(op == 4){
        if(num2 == 0) return -1; // 0으로 나누기 금지
        ret = num1 / num2;
    }
    if(ret > 999 || ret < 0) return -1; // 범위이탈 금지
    return ret;
}
// num = 앞 피연산자. 지금까지의 계산 누적결과 ex)a + b 에서 a
// pushCnt = 현재까지 누른 횟수
void DFS( int num, int pushCnt){
    if(pushCnt >= M) return; // M이 가변인 관계로 여기서도 해준다
    if(pushCnt <= M - 1){ // 마지막 = 을 눌러야하니까 -1의 여유
        if( num == target){
            suc = true;
            M = pushCnt+1; // = 을 눌렀다 치고 +1해준다
            return;
        }
    }

    for(int dg = 1; dg <= 3; dg++){ // 자릿수
        // 다음 누름횟수는 현재까지 누른 횟수+ 누를숫자 자릿수 + 연산자1개
        int nextPushCnt = pushCnt + dg + 1;
        if(nextPushCnt >= M) return;
        int Size = digits[dg].size();
        for(int op_i = 0; op_i < Size; op_i++){
            for(int o = 0; o < O; o++){
                // num2 : 뒤 피연산자 ex)a + b 에서 b
                int num2 = oper(num, digits[dg][op_i], oparts[o]);
                if(num2 != -1
                   && pushMemo[num2] > nextPushCnt){
                    pushMemo[num2] = nextPushCnt;
                    DFS(num2, nextPushCnt);
                }
            }
        }
    }
}
inline void init(){
    for(register int i = 0; i < 1000; i++) pushMemo[i] = INF;
    for(register int i = 1; i < 5; i++) useable_op[i] = false;
    digits[1].clear();
    digits[2].clear();
    digits[3].clear();
    oparts.clear();

    target = 0;
    suc = false;
}
int main() {
    //freopen("input.txt", "r", stdin);
    int TC = 1; scanf(" %d", &TC);
    for (int tc = 1; tc <= TC; tc++) {
        scanf(" %d %d %d", &N, &O, &M);
        init();
        int _min = 1000, _max = -1;
        for(register int n = 0; n < N; n++){
            register int inp;
            scanf(" %d", &inp);
            digits[1].push_back(inp);
            pushMemo[inp] = 1;
            if(_min > inp) _min = inp;
            if(_max < inp) _max = inp;
        }
        for(register int o = 0; o < O; o++){
            register int inp;
            scanf(" %d", &inp);
            oparts.push_back(inp);
            useable_op[inp] = true;
        }

        char ttmp[6];
        int tsize = 0;
        scanf("%s", ttmp);
        bool fullmatch = true;
        for(register int i = 0; ttmp[i]; i++){
            register int t = ttmp[i] - '0';
            if(pushMemo[t] == INF) fullmatch = false;
            tsize++;
            target = ((10*target) + t);
        }

        // only 숫자터치만으로 판별 시작
        if(fullmatch){
            // M 이 최소 4가 보장되고 최대 3자리라서
            // 여기 들어오면 무조건 가능
            printf("#%d %d\n", tc, tsize);
            continue;
        }
        // only 숫자터치만으로 판별은 여기까지
        //const char conv[5] = {'=', '+', '-', '*', '/'};
        // 증감한계 1차판단 시작
        if(target == 0){
            // 여기까지 왔으면 0버튼은 못누른다고 보면된다.
            // -> 위에서 이미 선행가지치기 당함
            if(useable_op[2]){
                printf("#%d 4\n", tc);
                continue;
            }else if(useable_op[4]){
                if(N == 1) printf("#%d 5\n",tc);
                else printf("#%d 4\n",tc);
                continue;
            }
        }

        if(!useable_op[1] && !useable_op[3]){ // + 와 *이 둘다 없으면
            // _mmmax : 숫자터치만으로 만들 수 있는 가장 큰 숫자
            // 세자리수여야 하니까 100 10 1
            register int _mmmax = _max*100 + _max * 10 + _max*1;
            if(_mmmax < target){ // _mmmax 위로는 만들 수 없다
                printf("#%d -1\n", tc);
                continue;
            }
        }

        if(!useable_op[2] && !useable_op[4]){ // - 와 /이 둘다 없으면
            // _min : 숫자터치만으로 만들 수 있는 가장 작은 숫자
            if(_min > target){ // _min 아래로는 만들 수 없다
                printf("#%d -1\n", tc);
                continue;
            }
        }
        // 증감 한계 1차판단은 여기까지

        // 누를수 있는 숫자버튼 조합을 전부 구한다
        int Size = digits[1].size();
        for(register int i = 0; i < Size; i++){
            int num = digits[1][i];
            for(register int j = 0; j < Size; j++){
                int np = digits[1][j];
                int num2 = num*10 + np;
                if( pushMemo[num2] > 2){ // 위에서 1을 눌렀는데 여기서 01을 누르는 경우를 회피
                    pushMemo[num2] = 2;
                    digits[2].push_back(num2);
                }
                for(register int k = 0; k < Size; k++){
                    int np2 = digits[1][k];
                    int num3 = num2 * 10 + np2;
                    if(pushMemo[num3] > 3){ // 이하 동문
                        pushMemo[num3] = 3;
                        digits[3].push_back(num3);
                    }
                }
            }
        }

        // 처음부터 연산없이 숫자버튼만 눌러서 가능한 경우는
        // 위에서 이미 가지치기 당했다.
        // 이제 남은건 무조건
        // 숫자]  (연산자 1회 + 숫자1회)k회  [ = 의 조합뿐이다
        // 그러니까 처음에 숫자를 던져놓고,
        // 재귀함수 안에서 (연산자1회 + 숫자1회)를 완탐한다
        for(int d = 1; d <= 3; d++){ // 자릿수별로 던져놓고 시작
            int Size = digits[d].size();
            for(int s = 0; s < Size; s++){
                DFS(digits[d][s],d);
            }
        }

        if(suc) printf("#%d %d\n", tc, M);
        else printf("#%d -1\n", tc);
    }

    return 0;
}
/*
 만약 입력 숫자나
     계산 도중의 결과가
       음수가 되거나
       999를 넘어가면
         문제가 발생한다.

두 자리 이상 숫자를
  입력하고 싶다면
  연속해서 숫자를 터치하면 된다.
  -> 41을 만들고 싶다면 ‘4’’1’을 입력하면 된다.

연산은 순차적으로 이루어진다.
‘2’, ‘+’, ‘2’, ‘*’, ‘3’, ‘=’가 입력된다면
  먼저 들어온 연산자부터 계산되어 12가 된다.
  -> 2 + 2 = 4, 4 * 3 = 12

나누기는
  소수 이하의 부분은 버리고
  정수 부분만 남는다.
  -> ‘7’, ‘/’, ‘3’, ‘=’ 가 입력된다면 2가 된다.
  -> ‘1’, ‘/’, ‘5’, ‘=’ 라면 결과는 0이 된다.
  ->


0으로 나눠서는 아니된다.
터치 횟수가 M을 넘어서는 아니 된다.
터치 횟수가 M이 될 때까지
  원하는 숫자 W를 만들 수 없다면
  -1을 출력한다.

터치 가능한 숫자는 N가지 ( 1 <= N <= 10)
터치 가능한 연산자들의 개수는 O 가지 (1 <= O <= 4)
최대 터치 횟수 M. ( 4 <= M <= 20)

연산자
[0] = '='; // 언제나 사용 가능
[1] = '+';
[2] = '-';
[3] = '*';
[4] = '/';
-----------------------
테케별 입력
N O M
터치가능한 숫자 N 개
터치가능한 연산자 O 개
원하는 숫자
----------------------

M은 사실상 M-1과 다름없다 ( '=' 때문)
a / b = c 일때
(b * c) <= a < (b * (c+1) )

터치 m1 + m2 로 만들수 있는 경우의 수
m1 = 3*(1,2,3)
m2 = M - m1 - 1

--- 선행 가지치기 -----
원하는 타겟을 단일문자로 하나씩 받아서
  가능한 숫자가 전부 존재하면 딱 자릿수만큼

'+' 와 '*' 이 둘다 없으면
  숫자터치만으로 만들 수 있는 가장 큰 숫자 위로는 만들 수 없다
'-' 와 '/' 이 둘다 없으면
  숫자터치만으로 만들 수 있는 가장 작은 숫자 아래로는 만들 수 없다

애초에 목적이 0이 아닌 이상 중간에 0을 만들 필요가 없다
-> 목적이 0인 경우만 따로 빼서 먼저 판단하면?
-> 나눗셈,곱셈,뺄셈 등에서 0이 나오는 걸 가지치기 할 수 있다


1. 순수하게 입력 가능한 숫자만으로 조합 가능한 숫자
2. 그 외 연산이 필요한 숫자
3. 1에 해당되지만, 2가 더 터치 횟수가 적은 숫자
  -> 3에 해당하는 케이스가 있나????

----선행 가지치기 종료
선행 가지치기는 속도 향상보다
  특수한 상황을 미리 배제해놓고 나중에 따지지 않기 위함

여기까지 고려했으면 나머지는 완탐이다


 */
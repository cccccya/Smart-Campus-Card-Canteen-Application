# Smart-Campus-Card-Canteen-Application
华中科技大学网络空间安全学院程序设计综合课程设计
## 项目需求
课程要求设计一个简单的校园卡管理系统，并能够执行以下操作：
1. 开户：输入姓名和学号。如果系统中没有该学号的账户，则将新建一个账户，并存入姓名和学号。
2. 销户：输入学号。如果系统中有该学号的账户，则将该账户删除。
3. 发卡：输入学号。如果系统中有该学号的账户且该账户没有卡，按照“3+流水号+校验码”的规则获得卡号，将该卡挂在账户下，如果系统中有该学号的账户且该账户已办理过卡，则转到补卡操作。
4. 挂失：输入学号。如果系统中有该学号的账户且该账户有可用卡，则把卡设置为挂失状态。
5. 解挂：输入学号。如果系统中有该学号的账户且该账户有被挂失的卡，则把卡挂失状态解除，设置为正常。
6. 补卡：输入学号。如果系统中有该学号的账户且该账户办理过卡，不论用户当前卡片是否可用，都按规则新开一张卡，并将上张卡的相关信息录入该卡中。
7. 充值：输入学号和充值金额。如果系统中有该学号的账户且该账户有可用卡，并且充值后金额不会超过1000元，则将钱充入账户中。
8. 消费：输入窗口号，卡号和消费金额。如果该卡可用，且卡中余额足够支付本次消费，则扣除卡中相应金额。输出该窗口当天的消费金额和消费次数。
9. 查询消费记录：输入学号，起始日期和截至日期。如果该账户存在，统计起止日期时间段的消费金额总和。输出消费金额总和。
10. 查询：输入姓名或者学号，此处支持模糊查询，‘?’表示一个字符，‘*’表示无，单个字符或多个字符。输出系统中满足格式要求的所有账户的学号姓名。
11. 数据汇总：将食堂99个窗口的消费数据通过多路归并或者其他排序方法，按照时间顺序进行排序。
12. 数据挖掘：输入某同学的学号。在数据汇总的基础上，使用某些方法获知该同学最有可能和哪一群人经常在相近时间、相邻窗口吃饭。输出这些同学的学号。
13. 数据校验：通过某些方法，确定系统中储存的数据内容未被修改，没有被删除或者伪造数据。
14. 文件导入：系统能够从指定文件中导入数据和各项操作记录。并且能按照正确的时间顺序执行数据中的操作。
15. 日志记录：每次操作过后需要保留日志文件，记录操作相关的各项信息。写入指定文件当中。

## 总体设计
![image](https://user-images.githubusercontent.com/77919385/223385973-3ae1e8c8-4e39-4459-b97b-cfd62cf007db.png)

## 数据结构设计
### 模糊查询——动态规划
状态定义：  
$dp[i][j]=1/0$ 表示匹配串的前 $i$ 个字符能/不能与原串的前 $j$ 个字符匹配。  
状态转移方程：  

匹配串第 $i$ 个字符为 $\ast$ ： $dp[i][j]=(dp[i-1][j]||dp[i][j-1]);$ //考虑 $\ast$ 表示为空或者直接添上原串 $j$ 位置字符。  

其他情况： $dp[i][j]=(dp[i-1][j-1]\And\And(matchStr[i-1]=='?'||originStr[j-1]==matchStr[i-1]));$ //前面部分能够完成匹配上并且这两个字符也能匹配

结果：  
$dp[MatchString.length()][OriginStr.length()]=1/0$ 表示两个字符串能/不能匹配。
![image](https://user-images.githubusercontent.com/77919385/223390024-9605292a-e103-4b84-a674-991b2c65e2f1.png)

### 汇总数据——多路归并/快排
多路归并：  
先将各窗口数据排好序放入数组中，在这里因为窗口是回绕的，可以直接找到窗口的首位置，从该位置开始将元素依次填入数组中，即可保证数组中元素有序。从所有窗口取出一个元素放入堆中。每次取出堆顶元素，放入答案数组当中。并将该元素所在窗口再取出一个元素放入堆中（窗口中已无元素则不执行该操作）。一直重复该操作直到堆中全部元素被取出。得到的序列即为有序。
![image](https://user-images.githubusercontent.com/77919385/223390477-966ed09e-827c-420c-ac51-d3ee21c44fa4.png)

快排：直接将各窗口数据汇总到一个数组当中。假设当前需要把[l,r]区间的数据排好序，取l位置的时间值作为标准，从l开始找大于该时间值的元素，从r开始找小于该时间值的元素，将两元素交换位置。再继续按上述规则进行交换，假设最后首尾指针都到了位置p，那么可以保证[l,p-1]均为小于等于该元素值的元素，[p+1,r]均为大于该元素值的元素，但是两区间内部还不能保证有序。于是再分别对[l,p-1]区间和[p+1,r]区间递归执行上述操作。最后区间大小为1是就能保证有序。
 ![image](https://user-images.githubusercontent.com/77919385/223390525-35cf22d0-b7a6-4dbc-a380-8842ebcf420f.png)

### 数据挖掘——Apriori算法。
Apriori是一种候选消除算法，它的大致流程如下：
1.	在原有数据中扫描一遍，对于单个元素，检查是否满足条件，清除不满足条件的元素，形成1级频繁级。
2.	若已经形成k-1级频繁集，考虑频繁集元素能形成的所有k元组，检查是否满足条件，清除不满足条件的元素，形成k级频繁集。
3.	重复2直至找不出n+1级频繁集，则n级频繁集为挖掘结果。
在程序中，我首先扫描一遍汇总数据，对于挖掘对象的消费记录，找出与该对象前后5min，左右2窗口进行过消费的全部同学。一次消费记录用一个map标记。对于一个k元组（可通过深搜获得），检查是否满足条件时，扫一遍挖掘对象的消费记录，在每一次消费记录的map中查询该组元素是否全在map中，如果都在，那么增加一个权重，如果最终权重超过20，则该k元组符合条件，可纳入k级频繁集当中。对找出来的同学中，求出1级频繁集，2级频繁集和3级频繁集，最终取3级频繁集的结果为可能的朋友集合。
![image](https://user-images.githubusercontent.com/77919385/223390673-17585b61-2bca-4ee9-a1d1-d2b813c58ca1.png)

### 判断数据是否被添加，删除，篡改——Hash算法
将Message对象中储存的时间，卡号，学号等信息项整合成一条字符串，并对其进行Hash操作，形成的Hash值1可以判断数据是否被篡改。  
具体到我的处理来说，若当前Hash值为key，当前处理到字符的ASCII码c，那么更改Hash值为 $(key*528+c)%998244353$ 。直至整个字符串的字符都对Hash值进行过更新。  
在汇总过程中，一条数据的Hash值1与前一条数据的Hash值1进行Hash操作（第一条与最后一条进行操作），形成的Hash值2保留在该条数据中，当访问该数据时，检查该数据的Hash值2可以判断数据是否被添加或者删除。  
设该条数据Hash值为key1，上条数据的Hash值为last_key1，则Hash值2的计算方法为 $key2=(last_key1+913)*(key+528)%998244353$ 。



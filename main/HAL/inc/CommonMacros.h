#ifndef COMMON_MACRO_H
#define COMMON_MACRO_H
/**
  * @brief  变量监视器，当变量改变时触发一个事件
  * @param  now:被监控的变量(整形)
  * @param  func:事件触发回调函数(可以是赋值等其他语句)
  * @retval 无
  */
#define CM_VALUE_MONITOR(now,func)  \
do{                                 \
    static int last=(now);          \
    if(last!=(now))func,last=(now); \
}while(0)

/**
  * @brief  让一个变量以设计的步近接近指定值
  * @param  src:被控变量
  * @param  dest:被接近的值
  * @param  step:步长
  * @retval 无
  */
#define CM_VALUE_CLOSE_TO(src,dest,step) \
do{                                      \
    if((src)<(dest))(src)+=(step);       \
    else if((src)>(dest))(src)-=(step);  \
}while(0)

/**
  * @brief  让一个变量增加或者减去一个值，在大于等于最大值后从0开始，低于0后从最大值开始
  * @param  src:被控变量
  * @param  step:增加或者减少的值
  * @param  max:最大值
  * @retval 无
  */
#define CM_VALUE_STEP(src,step,max) ((src)=(((step)>=0)?(((src)+(step))%(max)):(((src)+(max)+(step))%(max))))

/**
  * @brief  让一个变量增加或者减去一个值，在大于最大值后从最小值开始，小于最小值后从最大值开始
  * @param  src:被控变量
  * @param  plus:增加的值
  * @param  min:最小值
  * @param  max:最大值
  * @retval 无
  */
#define CM_VALUE_PLUS(src,plus,min,max)           \
do{                                               \
    int __value_temp = (src);                     \
    __value_temp += (plus);                       \
    if(__value_temp<(min))__value_temp=(max);     \
    else if(__value_temp>(max))__value_temp=(min);\
    (src) = __value_temp;                         \
}while(0)

/**
  * @brief  非阻塞式间隔指定时间执行一个函数
  * @param  func:被执行函数(可以是赋值等其他语句)
  * @param  time:设定间隔时间(ms)
  * @retval 无
  */
#define CM_EXECUTE_INTERVAL(func,time)                  \
do{                                                     \
    static unsigned long lasttime=0;                    \
    if(millis()-lasttime>=(time))func,lasttime=millis();\
}while(0)

/**
  * @brief  将一个函数重复调用指定次数
  * @param  func:被调用函数(可以是赋值等其他语句)
  * @param  n:重复调用次数
  * @retval 无
  */
#define CM_EXECUTE_LOOP(func,n) for(unsigned long i=0;i<(n);i++)func

/**
  * @brief  执行一个函数在不超时的情况下直到函数的返回值为指定值
  * @param  func:被调用函数
  * @param  n:希望函数的返回值
  * @param  timeout:超时时间
  * @param  flag:外部提供变量，用于检查是否超时
  * @retval 无
  */
#define CM_EXECUTE_FUNC_WITH_TIMEOUT(func,n,timeout,flag) \
do{                                                       \
    volatile unsigned long start=millis();                \
    (flag)=false;                                         \
    while(millis()-start<(timeout)){                      \
        if(func==(n)){(flag)=true;break;}                 \
    }                                                     \
}while(0)

/**
  * @brief  函数只执行一次，常用于初始化
  * @param  func:被调用函数(也可以是赋值等其他语句)
  * @retval 无
  */
#define CM_EXECUTE_ONCE(func)\
do{\
    static bool isInit = false;\
    if(!isInit){func,isInit=true;}\
}while(0)

#endif
void Rx_Convert_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                   // 专用于R{{{x}}}的转换宏
                   # define R{{{x}}}_fromFloat(x) ((R{{{x}}}_t){ (x) *(1U<<(31-{{{x}}})) })
                   """;
    Console.WriteLine(code);
}
void Rx_Convert_offset_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                   // 专用于R{{{x}}}的转换宏(有偏移项)
                   # define R{{{x}}}_fromFloat_offset(x,y) ((R{{{x}}}_t){ (x)*(1U<<(31-{{{x}}})) + y })
                   """;
    Console.WriteLine(code);
}
void Rx_Add_Generate(int x,int count)
{
    if(x is not (>= 0 and <= 31))return;
    if(count is not (>= 2 and <= 6))return;
    string code2=$$$"""
                    //定点数加法，相当于x+y
                    static inline R{{{x}}}_t R{{{x}}}_add(R{{{x}}}_t x,R{{{x}}}_t y) {
                        return (R{{{x}}}_t){x.value+y.value};
                    }
                    """;
    string code3=$$$"""
                    //定点数加法，相当于x+y+z
                    static inline R{{{x}}}_t R{{{x}}}_add_3(R{{{x}}}_t x,R{{{x}}}_t y,R{{{x}}}_t z) {
                        return (R{{{x}}}_t){x.value+y.value+z.value};
                    }
                    """;
    string code4=$$$"""
                    //定点数加法，相当于x+y+z+u
                    static inline R{{{x}}}_t R{{{x}}}_add_4(R{{{x}}}_t x,R{{{x}}}_t y,R{{{x}}}_t z,R{{{x}}}_t u) {
                        return (R{{{x}}}_t){x.value+y.value+z.value+u.value};
                    }
                    """;
    string code5=$$$"""
                    //定点数加法，相当于x+y+z+u+v
                    static inline R{{{x}}}_t R{{{x}}}_add_5(R{{{x}}}_t x,R{{{x}}}_t y,R{{{x}}}_t z,R{{{x}}}_t u,R{{{x}}}_t v) {
                        return (R{{{x}}}_t){x.value+y.value+z.value+u.value+v.value};
                    }
                    """;
    string code6=$$$"""
                    //定点数加法，相当于x+y+z+u+v+w
                    static inline R{{{x}}}_t R{{{x}}}_add_6(R{{{x}}}_t x,R{{{x}}}_t y,R{{{x}}}_t z,R{{{x}}}_t u,R{{{x}}}_t v,R{{{x}}}_t w) {
                        return (R{{{x}}}_t){x.value+y.value+z.value+u.value+v.value+w.value};
                    }
                    """;
    switch (count)
    {
        case 2:
            Console.WriteLine(code2);
            break;
        case 3:
            Console.WriteLine(code3);
            break;
        case 4:
            Console.WriteLine(code4);
            break;
        case 5:
            Console.WriteLine(code5);
            break;
        case 6:
            Console.WriteLine(code6);
            break;
    }
}
void Rx_Sub_Generate(int x,int count)
{
    if(x is not (>= 0 and <= 31))return;
    if(count is not (>= 2 and <= 3))return;
    string code2=$$$"""
                     //定点数减法，相当于x-y
                     static inline R{{{x}}}_t R{{{x}}}_sub(R{{{x}}}_t x,R{{{x}}}_t y) {
                         return (R{{{x}}}_t){x.value-y.value};
                     }
                     """;
    string code3=$$$"""
                     //定点数减法，相当于x-y-z
                     static inline R{{{x}}}_t R{{{x}}}_sub_3(R{{{x}}}_t x,R{{{x}}}_t y,R{{{x}}}_t z) {
                         return (R{{{x}}}_t){x.value-y.value-z.value};
                     }
                     """;
    switch (count)
    {
        case 2:
            Console.WriteLine(code2);
            break;
        case 3:
            Console.WriteLine(code3);
            break;
    }
}

void Rx_fxCompare_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                   //定点数判断相等，相当于x==y
                   static inline int32_t R{{{x}}}_equal(R{{{x}}}_t x,R{{{x}}}_t y) {
                       return x.value==y.value;
                   }
                   //定点数判断小于，相当于x<y
                   static inline int32_t R{{{x}}}_less(R{{{x}}}_t x,R{{{x}}}_t y) {
                       return x.value<y.value;
                   }
                   //定点数判断小于等于，相当于x<=y
                   static inline int32_t R{{{x}}}_less_equal(R{{{x}}}_t x,R{{{x}}}_t y) {
                       return x.value<=y.value;
                   }
                   //定点数判断大于，相当于x>y
                   static inline int32_t R{{{x}}}_greater(R{{{x}}}_t x,R{{{x}}}_t y) {
                       return x.value>y.value;
                   }
                   //定点数判断大于等于，相当于x>=y
                   static inline int32_t R{{{x}}}_greater_equal(R{{{x}}}_t x,R{{{x}}}_t y) {
                       return x.value>=y.value;
                   }
                   """;
    Console.WriteLine(code);
}
void Rx_floatCompare_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                    //定点数浮点数判断相等，相当于x==y
                    static inline int32_t R{{{x}}}_float_equal(R{{{x}}}_t x,float y) {
                        return x.value==(int32_t)(y*(float)(1U<<(31-({{{x}}}))));
                    }
                    //定点数浮点数判断小于，相当于x<y
                    static inline int32_t R{{{x}}}_float_less(R{{{x}}}_t x,float y) {
                        return x.value<(int32_t)(y*(float)(1U<<(31-({{{x}}}))));
                    }
                    //定点数浮点数判断小于等于，相当于x<=y
                    static inline int32_t R{{{x}}}_float_less_equal(R{{{x}}}_t x,float y) {
                        return x.value<=(int32_t)(y*(float)(1U<<(31-({{{x}}}))));
                    }
                    //定点数浮点数判断大于，相当于x>y
                    static inline int32_t R{{{x}}}_float_greater(R{{{x}}}_t x,float y) {
                        return x.value>(int32_t)(y*(float)(1U<<(31-({{{x}}}))));
                    }
                    //定点数浮点数判断大于等于，相当于x>=y
                    static inline int32_t R{{{x}}}_float_greater_equal(R{{{x}}}_t x,float y) {
                        return x.value>=(int32_t)(y*(float)(1U<<(31-({{{x}}}))));
                    }
                    """;
    Console.WriteLine(code);
}

void Rx_mul_Generate(int x,int y,int dst)
{
    if(x is not (>= 0 and <= 31))return;
    if(y is not (>= 0 and <= 31))return;
    if(dst is not (>= 0 and <= 31))return;
    string code=$$"""
                  //定点数乘法，相当于x*y
                  static inline R{{dst}}_t R{{x}}_{{y}}_{{dst}}_mul(R{{x}}_t x,R{{y}}_t y) {
                      return (R{{dst}}_t){(int32_t)(((int64_t)x.value*y.value)>>(31+{{dst}}-({{x}}+{{y}})))};
                  }
                  """;
    Console.WriteLine(code);
}
void Rx_floatMul_Generate(int x,int y,int dst)
{
    if(x is not (>= 0 and <= 31))return;
    if(y is not (>= 0 and <= 31))return;
    if(dst is not (>= 0 and <= 31))return;
    string code=$$"""
                   //定点数浮点数指定定点精度乘法，相当于x*y
                   static inline R{{dst}}_t R{{x}}_f{{y}}_{{dst}}_mul(R{{x}}_t x,float y) {
                       return (R{{dst}}_t){(int32_t)(((int64_t)x.value*((int32_t)(y*(float)(1U<<(31-{{y}})))))>>(31+{{dst}}-({{x}}+{{y}})))};
                   }
                   """;
    Console.WriteLine(code);
}
void Rx_floatAdd_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                   //定点数浮点数加法，相当于x+y
                   static inline R{{{x}}}_t R{{{x}}}_float_add(R{{{x}}}_t x,float y) {
                       return (R{{{x}}}_t){x.value+(int32_t)(y*(1U<<(31-({{{x}}}))))};
                   }
                   """;
    Console.WriteLine(code);
}


void R_sinTable_Generate()
{
    string table = "";
    for (int i = 0; i < 1024; i++)
    {
        double sinTable_double = Math.Round(Math.Sin(Math.PI * i / 2 / 1024) * Math.Pow(2, 31 - 0));
        if (sinTable_double >= Math.Pow(2, 31 - 0)) sinTable_double--;//如果超出范围则减一
        table+="{"+(Int32)(sinTable_double)+"}"+",";
        if (i % 100 == 0&&i!=0) table += "\n";
    }
    Console.WriteLine(table);
}
void Rx_Neg_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                   //定点数取反，相当于-x
                   static inline R{{{x}}}_t R{{{x}}}_neg(R{{{x}}}_t x) {
                       x.value=-x.value;
                       return x;
                   }
                   """;
    Console.WriteLine(code);
}
void Rx_ToInt32_Generate(int x)
{
    if(x is not (>= 0 and <= 31))return;
    string code=$$$"""
                   //定点数转整数，事实上该函数只用于SVPWM
                   static inline int32_t R{{{x}}}_toInt32(R{{{x}}}_t x) {
                       return x.value>>(31-{{{x}}});
                   }
                   """;
    Console.WriteLine(code);
}


//Rx_Convert_Generate(22);
//Rx_Add_Generate(22, 2);
//Rx_floatMul_Generate(4, 2, 6);
Rx_Neg_Generate(2);
//Rx_Add_Generate(0,5);
//R_sinTable_Generate();
//Rx_Sub_Generate(2,3);
//Rx_mul_Generate(15,0,2);
//Rx_floatCompare_Generate(4);
//Rx_ToInt32_Generate(15);
//Rx_floatAdd_Generate(0);
//Rx_fxCompare_Generate(4);


//name:PI控制器名称，inputR:输入定点类型，outputMIN~outputMAX输出最小值和最大值，outputR:输出定点类型，P:比例增益，PR:比例增益类型
//ITs:积分增益乘以执行周期，ITsR:积分增益类型，IAddR，积分累计项类型(AddUp的类型)
void PI_Controller_Generate(string name,int inputR,float outputMIN,float outputMAX,int outputR,float P,int PR,float ITs,int ITsR,int IAddR)
{
    string shift = IAddR > outputR ? $">>{IAddR-inputR}" : $"<<{inputR-IAddR}";
    string code=$$$"""
                   struct PI_{{{name}}}_Controller_t {
                       R{{{inputR}}}_t Set;//设定值
                       R{{{inputR}}}_t Measure;//实测值
                       //error=Set-Measure
                       R{{{outputR}}}_t Output;//输出值
                       R{{{IAddR}}}_t AddUp;//积分值
                   };
                   //{{{name}}}的PI控制器，此代码为自动生成的代码，各项参数如下:
                   //比例增益P:{{{P}}},积分增益乘以时间ITs:{{{ITs}}},输出最小最大值:{{{outputMIN}}}~{{{outputMAX}}}
                   static inline void {{{name}}}_PI_update(struct PI_{{{name}}}_Controller_t* {{{name}}}){
                        R{{{inputR}}}_t error=R{{{inputR}}}_sub({{{name}}}->Set,{{{name}}}->Measure);
                        R{{{outputR}}}_t output_unsat =R{{{outputR}}}_add(
                                R{{{PR}}}_{{{inputR}}}_{{{outputR}}}_mul(R{{{PR}}}_fromFloat({{{P}}}),error) ,
                                R{{{ITsR}}}_{{{IAddR}}}_{{{outputR}}}_mul(R{{{ITsR}}}_fromFloat({{{ITs}}}),{{{name}}}->AddUp)
                                );
                        R{{{outputR}}}_t output;       
                        bool saturated = false;  
                        if (R{{{outputR}}}_greater(output_unsat,R{{{outputR}}}_fromFloat({{{outputMAX}}}))) {       
                            output = R{{{outputR}}}_fromFloat({{{outputMAX}}});    
                            saturated = true;           
                        } else if (R{{{outputR}}}_less(output_unsat,R{{{outputR}}}_fromFloat({{{outputMIN}}}))) {
                            output = R{{{outputR}}}_fromFloat({{{outputMIN}}});    
                            saturated = true;        
                        } else {                         
                        output = output_unsat;           
                        }                                
                        if (!saturated) {    
                            {{{name}}}->AddUp.value+=error.value{{{shift}}};        
                        }                                
                        {{{name}}}->Output=output;             
                   }
                   extern struct PI_{{{name}}}_Controller_t {{{name}}}_PIstate;
                   """;
    Console.WriteLine(code);
}
// mode=0表示使用牛顿法，mode=1表示使用位移法
void Rx_sqrt_Generate64(int x, int mode = 0)
{
    // 此变量无用，如果代码中不存在u64开方方法，请把该变量的值粘贴到你的代码中
    string codePlusU64 = """
                      //牛顿法对u64开方
                      static inline uint64_t sqrt_newton_u64(uint64_t n) {
                          if (n == 0) return 0;
                          if (n <= 3) return 1;
                      
                          // 简单初始值：避免从 n 开始（减少1~2次迭代）
                          uint64_t x = n;
                          if (n > 0xFFFFFFFFULL) x = 0x100000000ULL; // 2^32
                          else if (n > 0xFFFFULL) x = 0x10000ULL;    // 2^16
                          else x = 0x100ULL;
                      
                          uint64_t y = (x + n / x) >> 1;
                          while (y < x) {
                              x = y;
                              y = (x + n / x) >> 1;
                          }
                          return x;
                      }
                      //位移法对u64开方
                      static inline uint64_t sqrt_bitwise_u64(uint64_t n) {
                          if (n == 0) return 0;
                      
                          uint64_t root = 0;
                          uint64_t bit = 1ULL << 62;  // (2^31)^2 = 2^62
                      
                          // 对齐最高位
                          while (bit > n) {
                              bit >>= 2;
                          }
                      
                          // 逐位确定
                          while (bit != 0) {
                              if (n >= root + bit) {
                                  n -= root + bit;
                                  root = (root >> 1) + bit;
                              } else {
                                  root >>= 1;
                              }
                              bit >>= 2;
                          }
                      
                          return root;
                      }
                      //上述两函数由ai生成，已经过随机验证
                      /*
                      // 生成 [0, UINT64_MAX] 的随机 uint64_t
                      uint64_t rand_u64(void) {
                          return ((uint64_t)rand() << 32) | (uint64_t)rand();
                      }
                      
                      int main(void) {
                          // 初始化随机种子
                          srand((unsigned)time(NULL));
                      
                      
                          for (int i=0;i<10;++i) {
                              uint64_t n = rand_u64();
                              uint32_t ref = (uint32_t)sqrt((double)n);
                              uint32_t yours = sqrt_bitwise_u64(n);
                      
                              printf("测试值: %"PRIu64",标准函数输出:%u,测试函数输出:%u,\n", n,ref,yours);
                          }
                      }
                       */
                      """;
    string modeReplace = ( mode == 0 ? "sqrt_newton_u64" : "sqrt_bitwise_u64" );
    string code = $$"""
                    static inline R{{x}}_t R{{x}}_sqrt(R{{x}}_t x) {
                        if (x.value<0)return (R{{x}}_t){0};
                        return (R{{x}}_t){{{modeReplace}}(((uint64_t)x.value)<<(31-{{x}}))};
                    }
                    """;
    Console.WriteLine(code);
}
// mode=0表示使用牛顿法，mode=1表示使用位移法
void Rx_sqrt_Generate32(int x, int mode = 0)
{
    // 此变量无用，如果代码中不存在u32开方方法，请把该变量的值粘贴到你的代码中
    string codePlusU32 = """
                      //牛顿法对u32开方
                      static inline uint32_t sqrt_newton_u32(uint32_t n) {
                          if (n == 0) return 0;
                          if (n <= 3) return 1;
                      
                          // 初始值选择：避免从 n 开始（加速收敛）
                          uint32_t x = n;
                          if (n > 0xFFFFU) x = 0x10000U;    // 2^16（适用于 n 接近 2^32）
                          else if (n > 0xFFU) x = 0x100U;   // 2^8
                          else x = 0x10U;                    // 2^4
                      
                          uint32_t y = (x + n / x) >> 1;
                          while (y < x) {
                              x = y;
                              y = (x + n / x) >> 1;
                          }
                          return x;
                      }
                      //位移法对u32开方
                      static inline uint32_t sqrt_bitwise_u32(uint32_t n) {
                          if (n == 0) return 0;
                      
                          uint32_t root = 0;
                          uint32_t bit = 1U << 30;  // (2^15)^2 = 2^30，因为 (2^16)^2 = 2^32 超出 uint32 范围
                      
                          // 对齐最高有效平方位
                          while (bit > n) {
                              bit >>= 2;
                          }
                      
                          // 逐位确定平方根
                          while (bit != 0) {
                              if (n >= root + bit) {
                                  n -= root + bit;
                                  root = (root >> 1) + bit;
                              } else {
                                  root >>= 1;
                              }
                              bit >>= 2;
                          }
                      
                          return root;
                      }
                      //上述两函数由ai生成，已经过随机验证
                      /*
                      // 生成 [0, UINT32_MAX] 的随机 uint32_t
                      uint32_t rand_u32(void) {
                          return  (uint32_t)rand();
                      }
                      
                      int main(void) {
                          // 初始化随机种子
                          srand((unsigned)time(NULL));
                      
                      
                          for (int i=0;i<10;++i) {
                              uint32_t n = rand_u32();
                              uint32_t ref = (uint32_t)sqrt((double)n);
                              uint32_t yours = sqrt_newton_u32(n);
                      
                              printf("测试值: %"PRIu32",标准函数输出:%u,测试函数输出:%u,\n", n,ref,yours);
                          }
                      }
                       */
                      """;
    string modeReplace = ( mode == 0 ? "sqrt_newton_u32" : "sqrt_bitwise_u32" );
    string code="";
    if (x % 2 == 0)
    {
        code = $$"""
                 static inline R{{x}}_t R{{x}}_sqrt(R{{x}}_t x) {
                     if (x.value<0)return (R{{x}}_t){0};
                     return (R{{x}}_t){{{modeReplace}}(x.value>>1)<<((32-{{x}})/2)};
                 }
                 """;
    }
    else
    {
        code = $$"""
                 static inline R{{x}}_t R{{x}}_sqrt(R{{x}}_t x) {
                     if (x.value<0)return (R{{x}}_t){0};
                     return (R{{x}}_t){{{modeReplace}}(x.value)<<((31-{{x}})/2)};
                 }
                 """;
    }
    Console.WriteLine(code);
}

//Rx_Convert_Generate(3);
//Rx_mul_Generate(4,2,4);
//Rx_mul_Generate(4,4,8);
PI_Controller_Generate("Id",2,-6.5f,6.5f,4,10,4,0.01f*20/15,0,10);
PI_Controller_Generate("Iq",2,-6.5f,6.5f,4,10,4,0.01f*20/15,0,10);
//Rx_Sub_Generate(15, 2);
//Rx_mul_Generate(0, 18, 2);
//Rx_fxCompare_Generate(8);
PI_Controller_Generate("Speed",15,-1f,1f,2,0.0005f,0,0.0000003f*20,0,22);
//Rx_sqrt_Generate32(8);
//Rx_floatCompare_Generate(8);
//Rx_floatMul_Generate(0, 4, 0);


统计文件示例及讲解:

先看每一层的名字对应的意义：

ALL:        所有对象，包括sock和app
    SELF:       对象的构造-析构之间这段生命周期
    VALID:      对象的构造-标记GC这段生命周期
    Fsm名字:    在这个状态下的这段生命周期

GC:         所有被标记为回收的对象
    SELF:       对象的标记为GC-析构这段生命周期

Actor名字:  某种actor
    SELF:       对象的构造-析构之间这段生命周期
    VALID:      对象的构造-标记GC这段生命周期
    Fsm名字:    在这个状态下的这段生命周期

再看具体的统计数值:

STAT_TOTAL:     累积总访问量
STAT_ALIVE:     处于此生命周期内的对象数量  
STAT_10MS_REQ:  生命周期时长在这个时间段的对象个数
STAT_50MS_REQ:  ...
STAT_100MS_REQ: ...
STAT_200MS_REQ: ...
STAT_500MS_REQ: ...
STAT_MORE_REQ:  ...

{
    "ALL":{
        "CAppFsmFini":{
            "STAT_TOTAL":9998,
            "STAT_ALIVE":0,
            "STAT_10MS_REQ":9995,
            "STAT_50MS_REQ":3,
            "STAT_100MS_REQ":0,
            "STAT_200MS_REQ":0,
            "STAT_500MS_REQ":0,
            "STAT_MORE_REQ":0
        },
        "SELF":{
            "STAT_TOTAL":14999,
            "STAT_ALIVE":703,
            "STAT_10MS_REQ":0,
            "STAT_50MS_REQ":31,
            "STAT_100MS_REQ":77,
            "STAT_200MS_REQ":146,
            "STAT_500MS_REQ":436,
            "STAT_MORE_REQ":13606
        },
        "VALID":{
            "STAT_TOTAL":14998,
            "STAT_ALIVE":1,
            "STAT_10MS_REQ":14290,
            "STAT_50MS_REQ":702,
            "STAT_100MS_REQ":5,
            "STAT_200MS_REQ":0,
            "STAT_500MS_REQ":0,
            "STAT_MORE_REQ":0
        }
    },
    "GC":{
        "SELF":{
            "STAT_TOTAL":14997,
            "STAT_ALIVE":701,
            "STAT_10MS_REQ":0,
            "STAT_50MS_REQ":37,
            "STAT_100MS_REQ":72,
            "STAT_200MS_REQ":145,
            "STAT_500MS_REQ":440,
            "STAT_MORE_REQ":13602
        }
    }
}


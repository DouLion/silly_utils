//
// Created by dell on 2023/11/17.
//

#include "silly_pinyin.h"

const static int spell_value[] = {-20319, -20317, -20304, -20295, -20292, -20283, -20265, -20257, -20242, -20230, -20051, -20036, -20032, -20026, -20002, -19990, -19986, -19982, -19976, -19805, -19784, -19775, -19774, -19763, -19756, -19751, -19746,
                                  -19741, -19739, -19728, -19725, -19715, -19540, -19531, -19525, -19515, -19500, -19484, -19479, -19467, -19289, -19288, -19281, -19275, -19270, -19263, -19261, -19249, -19243, -19242, -19238, -19235, -19227, -19224,
                                  -19218, -19212, -19038, -19023, -19018, -19006, -19003, -18996, -18977, -18961, -18952, -18783, -18774, -18773, -18763, -18756, -18741, -18735, -18731, -18722, -18710, -18697, -18696, -18526, -18518, -18501, -18490,
                                  -18478, -18463, -18448, -18447, -18446, -18239, -18237, -18231, -18220, -18211, -18201, -18184, -18183, -18181, -18012, -17997, -17988, -17970, -17964, -17961, -17950, -17947, -17931, -17928, -17922, -17759, -17752,
                                  -17733, -17730, -17721, -17703, -17701, -17697, -17692, -17683, -17676, -17496, -17487, -17482, -17468, -17454, -17433, -17427, -17417, -17202, -17185, -16983, -16970, -16942, -16915, -16733, -16708, -16706, -16689,
                                  -16664, -16657, -16647, -16474, -16470, -16465, -16459, -16452, -16448, -16433, -16429, -16427, -16423, -16419, -16412, -16407, -16403, -16401, -16393, -16220, -16216, -16212, -16205, -16202, -16187, -16180, -16171,
                                  -16169, -16158, -16155, -15959, -15958, -15944, -15933, -15920, -15915, -15903, -15889, -15878, -15707, -15701, -15681, -15667, -15661, -15659, -15652, -15640, -15631, -15625, -15454, -15448, -15436, -15435, -15419,
                                  -15416, -15408, -15394, -15385, -15377, -15375, -15369, -15363, -15362, -15183, -15180, -15165, -15158, -15153, -15150, -15149, -15144, -15143, -15141, -15140, -15139, -15128, -15121, -15119, -15117, -15110, -15109,
                                  -14941, -14937, -14933, -14930, -14929, -14928, -14926, -14922, -14921, -14914, -14908, -14902, -14894, -14889, -14882, -14873, -14871, -14857, -14678, -14674, -14670, -14668, -14663, -14654, -14645, -14630, -14594,
                                  -14429, -14407, -14399, -14384, -14379, -14368, -14355, -14353, -14345, -14170, -14159, -14151, -14149, -14145, -14140, -14137, -14135, -14125, -14123, -14122, -14112, -14109, -14099, -14097, -14094, -14092, -14090,
                                  -14087, -14083, -13917, -13914, -13910, -13907, -13906, -13905, -13896, -13894, -13878, -13870, -13859, -13847, -13831, -13658, -13611, -13601, -13406, -13404, -13400, -13398, -13395, -13391, -13387, -13383, -13367,
                                  -13359, -13356, -13343, -13340, -13329, -13326, -13318, -13147, -13138, -13120, -13107, -13096, -13095, -13091, -13076, -13068, -13063, -13060, -12888, -12875, -12871, -12860, -12858, -12852, -12849, -12838, -12831,
                                  -12829, -12812, -12802, -12607, -12597, -12594, -12585, -12556, -12359, -12346, -12320, -12300, -12120, -12099, -12089, -12074, -12067, -12058, -12039, -11867, -11861, -11847, -11831, -11798, -11781, -11604, -11589,
                                  -11536, -11358, -11340, -11339, -11324, -11303, -11097, -11077, -11067, -11055, -11052, -11045, -11041, -11038, -11024, -11020, -11019, -11018, -11014, -10838, -10832, -10815, -10800, -10790, -10780, -10764, -10587,
                                  -10544, -10533, -10519, -10331, -10329, -10328, -10322, -10315, -10309, -10307, -10296, -10281, -10274, -10270, -10262, -10260, -10256, -10254};

// 395个字符串，每个字符串长度不超过6
const static char silly_pinyin_spell_dict[396][7] = {"a",     "ai",   "an",    "ang",   "ao",    "ba",     "bai",  "ban",   "bang",  "bao",   "bei",    "ben",  "beng", "bi",    "bian", "biao",  "bie",   "bin",   "bing",  "bo",    "bu",     "ca",
                                                     "cai",   "can",  "cang",  "cao",   "ce",    "ceng",   "cha",  "chai",  "chan",  "chang", "chao",   "che",  "chen", "cheng", "chi",  "chong", "chou",  "chu",   "chuai", "chuan", "chuang", "chui",
                                                     "chun",  "chuo", "ci",    "cong",  "cou",   "cu",     "cuan", "cui",   "cun",   "cuo",   "da",     "dai",  "dan",  "dang",  "dao",  "de",    "deng",  "di",    "dian",  "diao",  "die",    "ding",
                                                     "diu",   "dong", "dou",   "du",    "duan",  "dui",    "dun",  "duo",   "e",     "en",    "er",     "fa",   "fan",  "fang",  "fei",  "fen",   "feng",  "fo",    "fou",   "fu",    "ga",     "gai",
                                                     "gan",   "gang", "gao",   "ge",    "gei",   "gen",    "geng", "gong",  "gou",   "gu",    "gua",    "guai", "guan", "guang", "gui",  "gun",   "guo",   "ha",    "hai",   "han",   "hang",   "hao",
                                                     "he",    "hei",  "hen",   "heng",  "hong",  "hou",    "hu",   "hua",   "huai",  "huan",  "huang",  "hui",  "hun",  "huo",   "ji",   "jia",   "jian",  "jiang", "jiao",  "jie",   "jin",    "jing",
                                                     "jiong", "jiu",  "ju",    "juan",  "jue",   "jun",    "ka",   "kai",   "kan",   "kang",  "kao",    "ke",   "ken",  "keng",  "kong", "kou",   "ku",    "kua",   "kuai",  "kuan",  "kuang",  "kui",
                                                     "kun",   "kuo",  "la",    "lai",   "lan",   "lang",   "lao",  "le",    "lei",   "leng",  "li",     "lia",  "lian", "liang", "liao", "lie",   "lin",   "ling",  "liu",   "long",  "lou",    "lu",
                                                     "lv",    "luan", "lue",   "lun",   "luo",   "ma",     "mai",  "man",   "mang",  "mao",   "me",     "mei",  "men",  "meng",  "mi",   "mian",  "miao",  "mie",   "min",   "ming",  "miu",    "mo",
                                                     "mou",   "mu",   "na",    "nai",   "nan",   "nang",   "nao",  "ne",    "nei",   "nen",   "neng",   "ni",   "nian", "niang", "niao", "nie",   "nin",   "ning",  "niu",   "nong",  "nu",     "nv",
                                                     "nuan",  "nue",  "nuo",   "o",     "ou",    "pa",     "pai",  "pan",   "pang",  "pao",   "pei",    "pen",  "peng", "pi",    "pian", "piao",  "pie",   "pin",   "ping",  "po",    "pu",     "qi",
                                                     "qia",   "qian", "qiang", "qiao",  "qie",   "qin",    "qing", "qiong", "qiu",   "qu",    "quan",   "que",  "qun",  "ran",   "rang", "rao",   "re",    "ren",   "reng",  "ri",    "rong",   "rou",
                                                     "ru",    "ruan", "rui",   "run",   "ruo",   "sa",     "sai",  "san",   "sang",  "sao",   "se",     "sen",  "seng", "sha",   "shai", "shan",  "shang", "shao",  "she",   "shen",  "sheng",  "shi",
                                                     "shou",  "shu",  "shua",  "shuai", "shuan", "shuang", "shui", "shun",  "shuo",  "si",    "song",   "sou",  "su",   "suan",  "sui",  "sun",   "suo",   "ta",    "tai",   "tan",   "tang",   "tao",
                                                     "te",    "teng", "ti",    "tian",  "tiao",  "tie",    "ting", "tong",  "tou",   "tu",    "tuan",   "tui",  "tun",  "tuo",   "wa",   "wai",   "wan",   "wang",  "wei",   "wen",   "weng",   "wo",
                                                     "wu",    "xi",   "xia",   "xian",  "xiang", "xiao",   "xie",  "xin",   "xing",  "xiong", "xiu",    "xu",   "xuan", "xue",   "xun",  "ya",    "yan",   "yang",  "yao",   "ye",    "yi",     "yin",
                                                     "ying",  "yo",   "yong",  "you",   "yu",    "yuan",   "yue",  "yun",   "za",    "zai",   "zan",    "zang", "zao",  "ze",    "zei",  "zen",   "zeng",  "zha",   "zhai",  "zhan",  "zhang",  "zhao",
                                                     "zhe",   "zhen", "zheng", "zhi",   "zhong", "zhou",   "zhu",  "zhua",  "zhuai", "zhuan", "zhuang", "zhui", "zhun", "zhuo",  "zi",   "zong",  "zou",   "zu",    "zuan",  "zui",   "zun",    "zuo"};
const static char SillyPinyinSpellDict[396][7] = {"A",     "Ai",   "An",    "Ang",   "Ao",    "Ba",     "Bai",  "Ban",   "Bang",  "Bao",   "Bei",    "Ben",  "Beng", "Bi",    "Bian", "Biao",  "Bie",   "Bin",   "Bing",  "Bo",    "Bu",     "Ca",
                                                  "Cai",   "Can",  "Cang",  "Cao",   "Ce",    "Ceng",   "Cha",  "Chai",  "Chan",  "Chang", "Chao",   "Che",  "Chen", "Cheng", "Chi",  "Chong", "Chou",  "Chu",   "Chuai", "Chuan", "Chuang", "Chui",
                                                  "Chun",  "Chuo", "Ci",    "Cong",  "Cou",   "Cu",     "Cuan", "Cui",   "Cun",   "Cuo",   "Da",     "Dai",  "Dan",  "Dang",  "Dao",  "De",    "Deng",  "Di",    "Dian",  "Diao",  "Die",    "Ding",
                                                  "Diu",   "Dong", "Dou",   "Du",    "Duan",  "Dui",    "Dun",  "Duo",   "E",     "En",    "Er",     "Fa",   "Fan",  "Fang",  "Fei",  "Fen",   "Feng",  "Fo",    "Fou",   "Fu",    "Ga",     "Gai",
                                                  "Gan",   "Gang", "Gao",   "Ge",    "Gei",   "Gen",    "Geng", "Gong",  "Gou",   "Gu",    "Gua",    "Guai", "Guan", "Guang", "Gui",  "Gun",   "Guo",   "Ha",    "Hai",   "Han",   "Hang",   "Hao",
                                                  "He",    "Hei",  "Hen",   "Heng",  "Hong",  "Hou",    "Hu",   "Hua",   "Huai",  "Huan",  "Huang",  "Hui",  "Hun",  "Huo",   "Ji",   "Jia",   "Jian",  "Jiang", "Jiao",  "Jie",   "Jin",    "Jing",
                                                  "Jiong", "Jiu",  "Ju",    "Juan",  "Jue",   "Jun",    "Ka",   "Kai",   "Kan",   "Kang",  "Kao",    "Ke",   "Ken",  "Keng",  "Kong", "Kou",   "Ku",    "Kua",   "Kuai",  "Kuan",  "Kuang",  "Kui",
                                                  "Kun",   "Kuo",  "La",    "Lai",   "Lan",   "Lang",   "Lao",  "Le",    "Lei",   "Leng",  "Li",     "Lia",  "Lian", "Liang", "Liao", "Lie",   "Lin",   "Ling",  "Liu",   "Long",  "Lou",    "Lu",
                                                  "Lv",    "Luan", "Lue",   "Lun",   "Luo",   "Ma",     "Mai",  "Man",   "Mang",  "Mao",   "Me",     "Mei",  "Men",  "Meng",  "Mi",   "Mian",  "Miao",  "Mie",   "Min",   "Ming",  "Miu",    "Mo",
                                                  "Mou",   "Mu",   "Na",    "Nai",   "Nan",   "Nang",   "Nao",  "Ne",    "Nei",   "Nen",   "Neng",   "Ni",   "Nian", "Niang", "Niao", "Nie",   "Nin",   "Ning",  "Niu",   "Nong",  "Nu",     "Nv",
                                                  "Nuan",  "Nue",  "Nuo",   "O",     "Ou",    "Pa",     "Pai",  "Pan",   "Pang",  "Pao",   "Pei",    "Pen",  "Peng", "Pi",    "Pian", "Piao",  "Pie",   "Pin",   "Ping",  "Po",    "Pu",     "Qi",
                                                  "Qia",   "Qian", "Qiang", "Qiao",  "Qie",   "Qin",    "Qing", "Qiong", "Qiu",   "Qu",    "Quan",   "Que",  "Qun",  "Ran",   "Rang", "Rao",   "Re",    "Ren",   "Reng",  "Ri",    "Rong",   "Rou",
                                                  "Ru",    "Ruan", "Rui",   "Run",   "Ruo",   "Sa",     "Sai",  "San",   "Sang",  "Sao",   "Se",     "Sen",  "Seng", "Sha",   "Shai", "Shan",  "Shang", "Shao",  "She",   "Shen",  "Sheng",  "Shi",
                                                  "Shou",  "Shu",  "Shua",  "Shuai", "Shuan", "Shuang", "Shui", "Shun",  "Shuo",  "Si",    "Song",   "Sou",  "Su",   "Suan",  "Sui",  "Sun",   "Suo",   "Ta",    "Tai",   "Tan",   "Tang",   "Tao",
                                                  "Te",    "Teng", "Ti",    "Tian",  "Tiao",  "Tie",    "Ting", "Tong",  "Tou",   "Tu",    "Tuan",   "Tui",  "Tun",  "Tuo",   "Wa",   "Wai",   "Wan",   "Wang",  "Wei",   "Wen",   "Weng",   "Wo",
                                                  "Wu",    "Xi",   "Xia",   "Xian",  "Xiang", "Xiao",   "Xie",  "Xin",   "Xing",  "Xiong", "Xiu",    "Xu",   "Xuan", "Xue",   "Xun",  "Ya",    "Yan",   "Yang",  "Yao",   "Ye",    "Yi",     "Yin",
                                                  "Ying",  "Yo",   "Yong",  "You",   "Yu",    "Yuan",   "Yue",  "Yun",   "Za",    "Zai",   "Zan",    "Zang", "Zao",  "Ze",    "Zei",  "Zen",   "Zeng",  "Zha",   "Zhai",  "Zhan",  "Zhang",  "Zhao",
                                                  "Zhe",   "Zhen", "Zheng", "Zhi",   "Zhong", "Zhou",   "Zhu",  "Zhua",  "Zhuai", "Zhuan", "Zhuang", "Zhui", "Zhun", "Zhuo",  "Zi",   "Zong",  "Zou",   "Zu",    "Zuan",  "Zui",   "Zun",    "Zuo"};

std::string silly_pinyin::chinese_to_pinyin(const std::string &gbk_chinese)
{
    std::string s_result;

    try
    {
        // 循环处理字节数组
        const int length = gbk_chinese.length();
        for (int j = 0, chrasc = 0; j < length;)
        {
            // 非汉字处理
            if (gbk_chinese.at(j) >= 0 && gbk_chinese.at(j) < 128)
            {
                s_result += gbk_chinese.at(j);
                // 偏移下标
                j++;
                continue;
            }

            // 汉字处理
            chrasc = gbk_chinese.at(j) * 256 + gbk_chinese.at(j + 1) + 256;
            if (chrasc > 0 && chrasc < 160)
            {
                // 非汉字
                s_result += gbk_chinese.at(j);
                // 偏移下标
                j++;
            }
            else
            {
                // 汉字
                for (int i = (sizeof(spell_value) / sizeof(spell_value[0]) - 1); i >= 0; --i)
                {
                    // 查找字典
                    if (spell_value[i] <= chrasc)
                    {
                        s_result += SillyPinyinSpellDict[i];
                        break;
                    }
                }
                // 偏移下标 （汉字双字节）
                j += 2;
            }
        }  // for end
    }
    catch (std::exception &e)
    {
        SU_ERROR_PRINT("%s", e.what());
    }
    return s_result;
}

#ifndef DemoSoundH
#define DemoSoundH

/*#define NI_HAO_ZHU_REN      1
#define HAO_DE              2
#define ZHE_SHI_MI_MI       3
#define WO_SHIXIAO_TIAN_E   4
#define XIAO_PING_GUO       5*/

#define NI_HAO_ZHU_REN      0
#define HAO_DE              1
#define ZHE_SHI_MI_MI       2
#define WO_SHIXIAO_TIAN_E   3
#define XIAO_PING_GUO       4

typedef struct sound{
  uint32_t addr;
	uint32_t dataSize;
} soundPara_t;

/*soundPara_t xdata soundPara[4] = 
{
  {0,1584},
  {1584, 1008},
  {2592, 0}
};
*/

#endif


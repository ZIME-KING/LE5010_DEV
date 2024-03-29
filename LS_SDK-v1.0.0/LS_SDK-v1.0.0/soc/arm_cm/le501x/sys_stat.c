#include <string.h>
#include "sys_stat.h"

struct peri_stat_env
{
    bool uart1;
    bool uart2;
    bool uart3;
    bool spi1;
    bool spi2;
    bool i2c1;
    bool i2c2;
    bool adtimer1;
    bool bstimer1;
    bool gptimera1;
    bool gptimerb1;
    bool gptimerc1;
    bool pdm;
    bool dma;
    bool crypt;
    bool adc12b;
    bool trng;
    bool ecc;
} peri_stat;

bool app_event_status;

void app_status_set(bool stat)
{
    app_event_status = stat;
}

void uart1_status_set(bool stat)
{
    peri_stat.uart1 = stat;
}

void uart2_status_set(bool stat)
{
    peri_stat.uart2 = stat;
}

void uart3_status_set(bool stat)
{
    peri_stat.uart3 = stat;
}

void spi1_status_set(bool stat)
{
    peri_stat.spi1 = stat;
}

void spi2_status_set(bool stat)
{
    peri_stat.spi2 = stat;
}

void i2c1_status_set(bool stat)
{
    peri_stat.i2c1 = stat;
}

void i2c2_status_set(bool stat)
{
    peri_stat.i2c2 = stat;
}

void adtimer1_status_set(bool stat)
{
    peri_stat.adtimer1 = stat;
}

void bstimer1_status_set(bool stat)
{
    peri_stat.bstimer1 = stat;
}

void gptimera1_status_set(bool stat)
{
    peri_stat.gptimera1 = stat;
}

void gptimerb1_status_set(bool stat)
{
    peri_stat.gptimerb1 = stat;
}

void gptimerc1_status_set(bool stat)
{
    peri_stat.gptimerc1 = stat;
}

void pdm_status_set(bool stat)
{
    peri_stat.pdm = stat;
}

void dma_status_set(bool stat)
{
    peri_stat.dma = stat;
}

void crypt_status_set(bool stat)
{
    peri_stat.crypt = stat;
}

void adc12b_status_set(bool stat)
{
    peri_stat.adc12b = stat;
}

void ecc_status_set(bool stat)
{
    peri_stat.ecc = stat;
}

void trng_status_set(bool stat)
{
    peri_stat.trng = stat;
}

bool peri_status_busy()
{
    const struct peri_stat_env idle_stat = {0};
    if(memcmp(&peri_stat,&idle_stat,sizeof(struct peri_stat_env))==0)
    {
        return false;
    } else
    {
        return true;
    }
}

//#include "log.h"

//bool peri_status_busy()
//{
//    const struct peri_stat_env idle_stat = {0};

//		LOG_I("uart1;%d",peri_stat.uart1);
//		LOG_I("uart2;%d",peri_stat.uart2);
//		LOG_I("uart3;%d",peri_stat.uart3);
//		LOG_I("spi1;%d",peri_stat.spi1);
//		LOG_I("spi2;%d",peri_stat.spi2);
//		LOG_I("i2c1;%d",peri_stat.i2c1);
//		LOG_I("i2c2;%d",peri_stat.i2c2);
//		LOG_I("adtimer1;%d",peri_stat.adtimer1);
//		LOG_I("bstimer1;%d",peri_stat.bstimer1);
//		LOG_I("gptimera1;%d",peri_stat.gptimera1);
//		LOG_I("gptimerb1;%d",peri_stat.gptimerb1);
//		LOG_I("gptimerc1;%d",peri_stat.gptimerc1);
//		LOG_I("pdm;%d",peri_stat.pdm);
//		LOG_I("crypt;%d",peri_stat.crypt);
//		LOG_I("adc12b;%d",peri_stat.adc12b);
//		LOG_I("trng;%d",peri_stat.trng);
//		LOG_I("ecc;%d",peri_stat.ecc);

//if(memcmp(&peri_stat,&idle_stat,sizeof(struct peri_stat_env))==0)
//{
//    return false;
//} else
//{
//    return true;
//}
//}





bool app_event_status_busy()
{
    return app_event_status;
}

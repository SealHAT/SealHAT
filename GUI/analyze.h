#ifndef ANALYZE_H
#define ANALYZE_H

#define STORAGECAPACITY         (8000000000.0)

#define	TOHOUR				3600.0
#define I2C_Speed           1.0/400000
#define SPI_SPEED           1.0/1000000

#define TEMP_CONV_TIME      7.0/1000
#define TEMP_CONV_PWR       90/1000000
#define TEMP_BIT_NUM        16.0
#define TEMP_SB_PWR         0.06/1000000
#define TEMP_I2C_PWR        3.5/1000

#define LIGHT_BIT_NUM       16.0
#define LIGHT_INACT_PWR     0.65/1000000
#define LIGHT_ACT_PWR       0.0000016

#define IMU_SB_PWR          2.0/1000000

#define EKG_I_AVDV          100.0/1000000
#define EKG_I_OV            0.6/1000000
#define EKG_I_SAVDV         0.51/1000000
#define EKG_I_SOV           1.1/1000000
#define EKG_OSCILLATOR      (1.4/1000000)*(24.0)

#define floatDebug() qDebug() << fixed << qSetRealNumberPrecision(10)

#define GPS_ACQ_PWR     30.0/1000
#define GPS_SB_PWR      300.0/1000000

#define SPI_CURRENT     20.0/1000
#define SPI_SB_CURRENT  15.0/1000000

#define MICRO_ACT_PWR   32.0/1000000
#define MICRO_SB_PWR    (13.0)/1000000 //

#endif // ANALYZE_H

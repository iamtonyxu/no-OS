#ifndef ERROR_CODE_T_H
#define ERROR_CODE_T_H

typedef enum {
    TXQEC_INIT_ERR_NO_ERROR           = 0,  /*!< 0 - No error */
    TXQEC_INIT_ERR_CAP_FAIL           = 1,  /*!< 1 - Capture error */
    TXQEC_INIT_ERR_CORR_FAIL          = 2,  /*!< 2 - Correlation error */
    TXQEC_INIT_ERR_NO_PATHDELAY       = 3,  /*!< 3 - No pathdelay estimate present */
    TXQEC_INIT_ERR_NCO_LOCK           = 4,  /*!< 4 - NCO failed to lock */
    TXQEC_INIT_HW_ERR                 = 5,  /*!< 5 - System h/w error */
    TXQEC_INIT_ERR_CORR_TIMEOUT_ABORT = 6,  /*!< 6 - Correlation capture timed-out or was aborted */
    TXQEC_INIT_HD2_ERR                = 7,  /*!< 7 - HD2 block error */
} ErrorCode_e;

#endif // ERROR_CODE_T_H
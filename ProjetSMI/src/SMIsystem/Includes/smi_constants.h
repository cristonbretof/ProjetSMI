#ifndef SMISYSTEM_INCLUDES_SMI_CONSTANTS_H_
#define SMISYSTEM_INCLUDES_SMI_CONSTANTS_H_

#define BIT_BASE    (uint32_t)(0x00000001)
#define BIT(n)      (uint32_t)(BIT_BASE << (n))

#define BITX(base, n)  (uint32_t)((base) << (n))

#endif /* SMISYSTEM_INCLUDES_SMI_CONSTANTS_H_ */

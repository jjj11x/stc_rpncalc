/*
 * key.h
 *
 *  Created on: Mar 20, 2019
 */

#ifndef SRC_KEY_H_
#define SRC_KEY_H_

void KeyInit(void);
void KeyScan(void);
const uint8_t* DebugGetKeys(void);
const uint32_t GetNewKeys(void);


#endif /* SRC_KEY_H_ */

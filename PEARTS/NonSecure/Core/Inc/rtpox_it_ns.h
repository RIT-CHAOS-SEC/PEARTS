/*
 * rtpox_it_ns.h
 *
 *  Created on: Mar 18, 2024
 *      Author: aj4775
 */

#ifndef INC_RTPOX_IT_NS_H_
#define INC_RTPOX_IT_NS_H_

#define RTPOX_NS_ELASTIC  __attribute__((section(".elastic_ns_section"),used))


void rtpox_dumb_it();

#endif /* INC_RTPOX_IT_NS_H_ */

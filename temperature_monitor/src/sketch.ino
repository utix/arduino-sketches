/****************************************************************************/
/*  Copyright © 2014-2015, Utix SAS                                         */
/*  All rights reserved.                                                    */
/*                                                                          */
/*  Redistribution and use in source and binary forms, with or without      */
/*  modification, are permitted provided that the following conditions are  */
/*  met:                                                                    */
/*      * Redistributions of source code must retain the above copyright    */
/*        notice, this list of conditions and the following disclaimer.     */
/*      * Redistributions in binary form must reproduce the above           */
/*        copyright notice, this list of conditions and the following       */
/*        disclaimer in the documentation and/or other materials provided   */
/*        with the distribution.                                            */
/*      * Neither the names of Utix employees nor the names of any          */
/*        contributors may be used to endorse or promote products derived   */
/*        from this software without specific prior written permission.     */
/*                                                                          */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     */
/*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT OWNER  */
/*  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,         */
/*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   */
/*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    */
/****************************************************************************/

#include <rgb.h>


#define LM35_PIN      A0
#define TEMP_PINS     5, 6, 7
#define VAR_PINS      10, 11, 12
#define SAMPLE_SIZE   32

#define countof(table)  sizeof(table) / sizeof((table)[0])
/*{{{ Globals */

static Rgb* led_temp;
static Rgb* led_var;

/* Temperatures sample in deciCelsius (200 for 20°C) */
static int temperatures[SAMPLE_SIZE];

/*}}}*/
/*{{{ Stats functions */

/*
 * Compute sample average
 * \param[in] t          the sample array
 * \param[in] size       the size of the sample
 *
 * \return float
 *    The average value of the sample
 */
static float average(int *t, size_t size) {
    float res = 0;

    if (!size)
        return 0;

    for (int i = 0; i < size; i++) {
        res += t[i];
    }

    return res / size;
}

/*
 * Compute variance of regular distributed sample
 *
 * \param[in] size       inumber of sample
 *
 * \return float
 *    The variance coefficient of the sample
 */
static float variance(size_t size) {
    float res = 0;
    float average = (size - 1) / 2.;

    if (!size)
        return 0;

    for (int i = 0; i < size; i++) {
        res += (i - average) * (i - average);
    }

    return res / size;
}

/*
 * Compute covariance of a sample regulary distributed on time
 *
 * \param[in] t          the sample array
 * \param[in] size       the size of the sample
 * \param[in] a          the average value of the sample
 * \param[in] pos        the lastest element of the sample
 *
 * \return float
 *    The covariance coefficient of the sample
 */
static float covariance(int *t, size_t size, float a, int pos) {
    float res = 0;
    float average = (size - 1) / 2.;

    if (!size)
        return 0;

    for (int i = 0; i < size; i++) {
        res += (t[(i + pos) % size] - a) * (i - average);
    }

    return res / size;
}

/*
 * Find the regression coefficient of a regulary distributed sample
 *
 * \param[in] t          the sample array
 * \param[in] size       the size of the sample
 * \param[in] a          the average value of the sample
 * \param[in] pos        the lastest element of the sample
 *
 * \return float
 *    The regression coefficient of the sample
 */
static float coefficient(int *t, size_t size, float a, int pos) {
    float var, covar;

    var = variance(size);
    covar = covariance(t, size, a, pos);

    return covar / var;
}

/*}}}*/

/*
 * This return 10 * the temperature in Celsius
 * Temperatures are stored as int, we need one decimal for precision
 *
 *
 * \return float
 *     Read the LMS35 value as deciCelsius
 */
static float get_temp() {
    /* 1.1V is the INTERNAL analogReference */
    /* 1000 to have deciCelsius
     * 1023 analogRead returns value between 0 and 1023
     */
    return analogRead(LM35_PIN) * 1.1 * 1000 / 1023;
}

void setup() {
    float temp;
    Serial.begin(9600);
    Serial.println("Coucou");
    led_temp = new Rgb(TEMP_PINS);
    led_var = new Rgb(VAR_PINS);

    /* use INTERNAL 1.1V as reference to increase accuracy */
    analogReference(INTERNAL);

    /* at startup fill the temp array with the current temp */
    temp = get_temp();
    for (int i = 0; i < countof(temperatures); i++) {
        temperatures[i] = temp;
    }
}

void loop() {
    static int i = 0;

    float temp;
    float coef;

    i %= countof(temperatures);

    temperatures[i++] = get_temp();

    temp = average(temperatures, countof(temperatures));

    /*
     * Use the regression coefficient to know if temp is increasing, stable
     * or decreasing
     * */
    coef = 100 * coefficient(temperatures, countof(temperatures), temp, i);

    Serial.print("Temp: ");
    Serial.print(" ");
    Serial.print(temp / 10.);
    Serial.print(" ");
    Serial.print(coef);
    Serial.println("°C");

    if (coef > 0) {
        led_var->color(RED);
    } else if (coef < 0) {
        led_var->color(AQUA);
    } else {
        led_var->color(GREEN);
    }
    if (temp < 200) {
        led_temp->color(AQUA);
    } else if (temp > 300) {
        led_temp->color(RED);
    } else if (temp < 240) {
        led_temp->color(GREEN);
    } else {
        led_temp->color(ORANGE);
    }
    delay(1000);
}


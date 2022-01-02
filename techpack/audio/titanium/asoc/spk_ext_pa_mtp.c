// SPDX-License-Identifier: GPL-2.0-only
//
// Copyright (C) 2015-2020 The Linux Foundation. All rights reserved.
//               2023      The LineageOS Project
//

#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <sound/soc.h>
#include "codecs/msm-cdc-pinctrl.h"
#include "msm8952.h"

struct cdc_pdm_pinctrl_info {
	struct pinctrl *pinctrl;
	struct pinctrl_state *spk_ext_pa_act;
	struct pinctrl_state *spk_ext_pa_sus;
};

static struct cdc_pdm_pinctrl_info pinctrl_info;

int msm_spk_ext_pa_ctrl(struct msm_asoc_mach_data *pdatadata, bool value)
{
	struct msm_asoc_mach_data *pdata = pdatadata;
	struct sched_param param;
	bool on_off = !value;
	int ret = 0;
	int maxpri;

	maxpri = MAX_USER_RT_PRIO - 1;
	pr_debug("whl apk pa ctl -> high priorty start priorty = %d\n", maxpri);
	param.sched_priority = maxpri;
	if (sched_setscheduler(current, SCHED_FIFO, &param) == -1)
		pr_debug("whl sched_setscheduler failed\n");
	pr_debug("whl apk pa ctl -> high priorty end\n");
	pr_debug("%s, pa_is_on=%d, spk_ext_pa_gpio_lc=%d, on_off=%d\n", __func__, pdata->pa_is_on, pdata->spk_ext_pa_gpio_lc, on_off);
	if (gpio_is_valid(pdata->spk_ext_pa_gpio_lc)) {
		ret = msm_cdc_pinctrl_select_active_state(pdata->mi2s_gpio_p[PRIM_MI2S]);
		if (ret) {
			pr_err("%s: gpio set cannot be de-activated %s\n",
					__func__, "pri_i2s");
			return ret;
		}
		if (on_off) {
			gpio_direction_output(pdata->spk_ext_pa_gpio_lc, 0);
			mdelay(2);
			pr_debug("111111 At %d In (%s), set pa\n", __LINE__, __FUNCTION__);
			gpio_set_value(pdata->spk_ext_pa_gpio_lc, 0);
			mdelay(2);
			gpio_set_value(pdata->spk_ext_pa_gpio_lc, 1);

			gpio_set_value(pdata->spk_ext_pa_gpio_lc, 0);
			gpio_set_value(pdata->spk_ext_pa_gpio_lc, 1);
			pr_debug("At %d In (%s), will delay\n", __LINE__, __FUNCTION__);
			msleep(3);
			printk("At %d In (%s), after open pa, spk_ext_pa_gpio_lc=%d\n", __LINE__, __FUNCTION__, gpio_get_value(pdata->spk_ext_pa_gpio_lc));

		} else {
			gpio_set_value(pdata->spk_ext_pa_gpio_lc, 0);
			printk("At %d In (%s), after close pa, spk_ext_pa_gpio_lc=%d\n", __LINE__, __FUNCTION__, gpio_get_value(pdata->spk_ext_pa_gpio_lc));
		}
	} else {
		pr_debug("%s, error\n", __func__);
		ret = -EINVAL;
	}

	return ret;
}

void msm_spk_ext_pa_delayed(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct msm_asoc_mach_data *pdata;

	dwork = to_delayed_work(work);
	pdata = container_of(dwork, struct msm_asoc_mach_data, pa_gpio_work);
	pr_debug("At %d In (%s), enter, pdata->pa_is_on=%d\n", __LINE__, __FUNCTION__, pdata->pa_is_on);

	if (!pdata->pa_is_on) {
		pr_debug("At %d In (%s), open pa\n", __LINE__, __FUNCTION__);
		msm_spk_ext_pa_ctrl(pdata, true);
		pdata->pa_is_on = 2;
	}
}

int msm_setup_spk_ext_pa(struct platform_device *pdev, struct msm_asoc_mach_data *pdata)
{
	struct pinctrl *pinctrl;

	pdata->spk_ext_pa_gpio_lc = of_get_named_gpio_flags(pdev->dev.of_node, "qcom,spk_ext_pa",
				0, NULL);
	if (pdata->spk_ext_pa_gpio_lc < 0) {
		pr_debug("%s, spk_ext_pa_gpio_lc not exist!\n", __func__);
	} else {
		pr_debug("%s, spk_ext_pa_gpio_lc=%d\n", __func__, pdata->spk_ext_pa_gpio_lc);
		pinctrl = devm_pinctrl_get(&pdev->dev);
		if (IS_ERR(pinctrl)) {
			pr_err("%s: Unable to get pinctrl handle\n", __func__);
			return -EINVAL;
		}
		pinctrl_info.pinctrl = pinctrl;
		/* get pinctrl handle for spk_ext_pa_gpio_lc */
		pinctrl_info.spk_ext_pa_act = pinctrl_lookup_state(pinctrl, "spk_ext_pa_active");
		if (IS_ERR(pinctrl_info.spk_ext_pa_act)) {
			pr_err("%s: Unable to get pinctrl active handle\n", __func__);
			return -EINVAL;
		}
		pinctrl_info.spk_ext_pa_sus = pinctrl_lookup_state(pinctrl, "spk_ext_pa_suspend");
		if (IS_ERR(pinctrl_info.spk_ext_pa_sus)) {
			pr_err("%s: Unable to get pinctrl disable handle\n", __func__);
			return -EINVAL;
		}
		if (gpio_is_valid(pdata->spk_ext_pa_gpio_lc)) {
			pr_debug("%s, spk_ext_pa_gpio_lc request\n", __func__);
			pr_debug("At %d In (%s), set spk_ext_pa_gpio_lc to low\n", __LINE__, __FUNCTION__);
			gpio_direction_output(pdata->spk_ext_pa_gpio_lc, 0);
			mdelay(2);
			gpio_set_value(pdata->spk_ext_pa_gpio_lc, 0);
		}
	}
	return 0;
}

static int external_spk_control = 1;

int get_external_spk_pa(struct snd_kcontrol *kcontrol,
		       struct snd_ctl_elem_value *ucontrol)
{
	pr_debug("At %d In (%s), external_spk_control=%d\n", __LINE__, __FUNCTION__, external_spk_control);
	ucontrol->value.integer.value[0] = external_spk_control;
	return 0;
}

int set_external_spk_pa(struct snd_kcontrol *kcontrol,
		       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_soc_kcontrol_codec(kcontrol);
	struct msm_asoc_mach_data *pdata = NULL;
	pdata = snd_soc_card_get_drvdata(codec->component.card);
	pr_debug("At %d In (%s), external_spk_control=%d, value.integer.value[0]=%ld\n", __LINE__, __FUNCTION__, external_spk_control, ucontrol->value.integer.value[0]);
	if (external_spk_control == ucontrol->value.integer.value[0])
		return 0;
	external_spk_control = ucontrol->value.integer.value[0];
	msm_spk_ext_pa_ctrl(pdata, external_spk_control);
	return 1;
}

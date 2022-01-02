// SPDX-License-Identifier: GPL-2.0-only
//
// Copyright (C) 2015-2020 The Linux Foundation. All rights reserved.
//               2023      The LineageOS Project
//

#ifndef __SPK_EXT_PA_MTP_H__
#define __SPK_EXT_PA_MTP_H__

#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <sound/soc.h>
#include "msm8952.h"

extern int msm_spk_ext_pa_ctrl(struct msm_asoc_mach_data *pdatadata, bool value);

extern void msm_spk_ext_pa_delayed(struct work_struct *work);

extern int msm_setup_spk_ext_pa(struct platform_device *pdev, struct msm_asoc_mach_data *pdata);

extern int get_external_spk_pa(struct snd_kcontrol *kcontrol,
                       struct snd_ctl_elem_value *ucontrol);

extern int set_external_spk_pa(struct snd_kcontrol *kcontrol,
                       struct snd_ctl_elem_value *ucontrol);

#endif /* __SPK_EXT_PA_MTP_H__ */

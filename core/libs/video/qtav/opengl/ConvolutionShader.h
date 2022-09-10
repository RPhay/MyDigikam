/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_CONVOLUTION_SHADER_H
#define QTAV_CONVOLUTION_SHADER_H

// Local includes

#include "VideoShader.h"

namespace QtAV
{

class ConvolutionShaderPrivate;

/*!
 * \brief The ConvolutionShader class
 * Uniform u_Kernel is used
 */
class DIGIKAM_EXPORT ConvolutionShader : public VideoShader
{
    DPTR_DECLARE_PRIVATE(ConvolutionShader)

public:

    ConvolutionShader();

    /*!
     * \brief kernelRadius
     * Default is 1, i.e. 3x3 kernel
     * kernelSize is (2*kernelRadius()+1)^2
     * \return
     */
    int kernelRadius()                      const;

    /// TODO: update shader program if radius is changed. mark dirty program

    void setKernelRadius(int value);
    int kernelSize()                        const;

protected:

    virtual const float* kernel()           const = 0;
    const QByteArray& kernelUniformHeader() const;  ///< can be used in your userFragmentShaderHeader();
    const QByteArray& kernelSample()        const;  ///< can be  in your userSample();
    void setKernelUniformValue();                   ///< can be used in your setUserUniformValues()

private:

    /// default implementions

    const char* userShaderHeader(QOpenGLShader::ShaderType t) const override;
    const char* userSample()                                  const override;
    bool setUserUniformValues()                                     override;

protected:

    ConvolutionShader(ConvolutionShaderPrivate& d);
};

} // namespace QtAV

#endif // QTAV_CONVOLUTION_SHADER_H

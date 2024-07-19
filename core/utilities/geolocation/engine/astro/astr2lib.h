/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "attlib.h"

#include "digikam_export.h"

namespace Marble
{

/**
 * Definitions of Astr2lib Functions
 */
class DIGIKAM_EXPORT Plan200      // Calculating the Planets in epoch J2000.0 coordinates
{
public:

    Plan200();
    Vec3 velocity();   // return last calculated planet velocity
    void state(Vec3& rs, Vec3& vs);   // return last state vector
    Vec3 Mercury(double t);    // position of Mercury at time t
    Vec3 Venus(double t);    // position of Venus at time t
    Vec3 Mars(double t);    // position of Mars at time t
    Vec3 Jupiter(double t);    // position of Jupiter at time t
    Vec3 Saturn(double t);    // position of Saturn at time t
    Vec3 Uranus(double t);    // position of Uranus at time t
    Vec3 Neptune(double t);    // position of Neptune at time t
    Vec3 Pluto(double t);    // position of Pluto at time t

private:

    double c3[19], s3[19];
    double c[11], s[11];
    double m1, m2, m3, m4, m5, m6, m7, m8;
    double /*d, a, uu,*/ tt;
    double u, v, dl, dr, db, l, b, r;
    Vec3   rp, vp;   // state vector of planet

private:

    void addthe(double c1, double s1, double c2, double s2,
                double& cc, double& ss);
    void term(int i1, int i, int it, double dlc, double dls, double drc,
              double drs, double dbc, double dbs);
    void posvel();
};

DIGIKAM_EXPORT void MarPhobos(double t, Vec3& rs, Vec3& vs);  // State vector of Phobos
DIGIKAM_EXPORT void MarDeimos(double t, Vec3& rs, Vec3& vs);  // State vector of Deimos
DIGIKAM_EXPORT Vec3 PosJIo(double t);       // Position of Io
DIGIKAM_EXPORT Vec3 PosEuropa(double t);    // Position of Europa
DIGIKAM_EXPORT Vec3 PosGanymede(double t);  // Position of Ganymede
DIGIKAM_EXPORT Vec3 PosCallisto(double t);  // Position of Callisto
DIGIKAM_EXPORT Vec3 PosSMimas(double t);  // Position of Mimas
DIGIKAM_EXPORT Vec3 PosSEnceladus(double t);  // Position of Enceladus
DIGIKAM_EXPORT Vec3 PosSDione(double t);  // Position of Dione
DIGIKAM_EXPORT void JupIo(double t, Vec3& rs, Vec3& vs);        // state vector of Io
DIGIKAM_EXPORT void JupEuropa(double t, Vec3& rs, Vec3& vs);    // state vector of Europa
DIGIKAM_EXPORT void JupGanymede(double t, Vec3& rs, Vec3& vs);  // state vector of Ganymede
DIGIKAM_EXPORT void JupCallisto(double t, Vec3& rs, Vec3& vs);  // state vector of Callisto
DIGIKAM_EXPORT void SatRhea(double t, Vec3& rs, Vec3& vs);    // state vector of Rhea
DIGIKAM_EXPORT void SatTitan(double t, Vec3& rs, Vec3& vs);   // state vector of Titan
DIGIKAM_EXPORT void NepTriton(double t, Vec3& rs, Vec3& vs);  // state vector of Triton
DIGIKAM_EXPORT void PluCharon(double t, Vec3& rs, Vec3& vs);  // state vector of Charon

} // namespace Marble

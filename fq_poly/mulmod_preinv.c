/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson
    Copyright (C) 2013 Martin Lee
    Copyright (C) 2013 Mike Hansen

******************************************************************************/

#include "fq_poly.h"

void _fq_poly_mulmod_preinv(fq_struct *res, fq_struct *poly1, slong len1,
                            fq_struct  *poly2, slong len2, fq_struct *f,
                            slong lenf, fq_struct *finv, slong lenfinv,
                            const fq_ctx_t ctx)
{
    fq_struct *T, *Q;
    slong lenT, lenQ;

    lenT = len1 + len2 - 1;
    lenQ = lenT - lenf + 1;

    T = _fq_vec_init(lenT + lenQ, ctx);
    Q = T + lenT;

    if (len1 >= len2)
        _fq_poly_mul(T, poly1, len1, poly2, len2, ctx);
    else
        _fq_poly_mul(T, poly2, len2, poly1, len1, ctx);

    _fq_poly_divrem_newton_preinv(Q, res, T, lenT, f, lenf,
                                  finv, lenfinv, ctx);
    _fq_vec_clear(T, lenT + lenQ);
}

void
fq_poly_mulmod_preinv(fq_poly_t res, const fq_poly_t poly1,
                      const fq_poly_t poly2, const fq_poly_t f,
                      const fq_poly_t finv, const fq_ctx_t ctx)
{
    slong len1, len2, lenf;
    fq_struct *fcoeffs;

    lenf = f->length;
    len1 = poly1->length;
    len2 = poly2->length;

    if (lenf == 0)
    {
        flint_printf("Exception (fq_poly_mulmod). Divide by zero.\n");
        abort();
    }

    if (lenf == 1 || len1 == 0 || len2 == 0)
    {
        fq_poly_zero(res, ctx);
        return;
    }

    if (len1 + len2 - lenf > 0)
    {
        if (f == res)
        {
            fcoeffs = _fq_vec_init(lenf, ctx);
            _fq_vec_set(fcoeffs, f->coeffs, lenf);
        }
        else
            fcoeffs = f->coeffs;

        fq_poly_fit_length(res, lenf - 1, ctx);
        _fq_poly_mulmod_preinv(res->coeffs, poly1->coeffs, len1,
                               poly2->coeffs, len2,
                               fcoeffs, lenf,
                               finv->coeffs, finv->length,
                               ctx);
        if (f == res)
            _fq_vec_clear(fcoeffs, lenf);

        res->length = lenf - 1;
        _fq_poly_normalise(res, ctx);
    }
    else
    {
        fq_poly_mul(res, poly1, poly2, ctx);
    }
}

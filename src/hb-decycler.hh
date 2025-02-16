/*
 * Copyright © 2025 Behdad Esfahbod
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author(s): Behdad Esfahbod
 */

#ifndef HB_DECYCLER_HH
#define HB_DECYCLER_HH

#include "hb.hh"

struct hb_decycler_node_t;

struct hb_decycler_t
{
  friend struct hb_decycler_node_t;

  private:
  hb_decycler_node_t *tortoise = nullptr;
  hb_decycler_node_t *hare = nullptr;
};

struct hb_decycler_node_t
{
  hb_decycler_node_t (hb_decycler_t &decycler)
    : decycler (decycler)
  {
    snapshot = decycler;

    if (!decycler.tortoise)
    {
      // First node.
      decycler.tortoise = decycler.hare = this;
      return;
    }

    tortoise_asleep = !decycler.hare->tortoise_asleep;
    decycler.hare->next = this;
    decycler.hare = this;

    if (!tortoise_asleep)
      decycler.tortoise = decycler.tortoise->next; // Time to move.
  }

  ~hb_decycler_node_t ()
  {
    decycler = snapshot;
  }

  bool visit (unsigned value_)
  {
    value = value_;

    if (decycler.tortoise == this)
      return true; // First node; not a cycle.

    if (decycler.tortoise->value == value)
      return false; // Cycle detected.

    return true;
  }

  private:
  hb_decycler_t &decycler;
  hb_decycler_t snapshot;
  hb_decycler_node_t *next = nullptr;
  unsigned value = (unsigned) -1;
  bool tortoise_asleep = false;
};

#endif /* HB_DECYCLER_HH */

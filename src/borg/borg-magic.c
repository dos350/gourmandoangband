/**
 * \file  borg-magic.c
 * \brief The basic magic definitions and routines to cast spells
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * Copyright (c) 2007-9 Andi Sidwell, Chris Carr, Ed Graham, Erik Osheim
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband License":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "borg-magic.h"

#ifdef ALLOW_BORG

#include "../effects.h"
#include "../player-spell.h"
#include "../ui-menu.h"

#include "borg.h"
#include "borg-cave.h"
#include "borg-cave-view.h"
#include "borg-init.h"
#include "borg-io.h"
#include "borg-trait.h"

/*
 * Spell info - individualized for class by spell number
*/

borg_magic *borg_magics = NULL;


static borg_spell_rating borg_spell_ratings_MAGE[] =
{
    { "Magic Missile", 95, MAGIC_MISSILE },
    { "Light Room", 65, LIGHT_ROOM },
    { "Find Traps, Doors & Stairs", 85, FIND_TRAPS_DOORS_STAIRS },
    { "Phase Door", 95, PHASE_DOOR },
    { "Electric Arc", 85, ELECTRIC_ARC },
    { "Detect Monsters", 85, DETECT_MONSTERS },
    { "Fire Ball", 75, FIRE_BALL },
    { "Recharging", 65, RECHARGING },
    { "Identify Rune", 95, IDENTIFY_RUNE },
    { "Treasure Detection", 5, TREASURE_DETECTION }, /* borg never uses this */
    { "Frost Bolt", 75, FROST_BOLT },
    { "Reveal Monsters", 85, REVEAL_MONSTERS },
    { "Acid Spray", 75, ACID_SPRAY },
    { "Disable Traps, Destroy Doors", 95, DISABLE_TRAPS_DESTROY_DOORS },
    { "Teleport Self", 95, TELEPORT_SELF },
    { "Teleport Other", 75, TELEPORT_OTHER },
    { "Resistance", 90, RESISTANCE },
    { "Resist Cold", 85, RESIST_COLD_SPELL },
    { "Resist Fire", 85, RESIST_FIRE_SPELL },
    { "Shield", 85, SHIELD_SPELL },
    { "Hold Monster", 55, HOLD_MONSTER },
    { "Tap Magical Energy", 5, TAP_MAGICAL_ENERGY }, /* need to figure out when to cast this one */
    { "Mana Channel", 95, MANA_CHANNEL },
    { "Door Creation", 65, DOOR_CREATION },
    { "Mana Bolt", 95, MANA_BOLT },
    { "Teleport Level", 65, TELEPORT_LEVEL },
    { "Detection", 95, DETECTION },
    { "Dimension Door", 95, DIMENSION_DOOR },
    { "Thrust Away", 55, THRUST_AWAY },
    { "Shock Wave", 85, SHOCK_WAVE },
    { "Explosion", 85, EXPLOSION },
    { "Banishment", 75, BANISHMENT },
    { "Mass Banishment", 65, MASS_BANISHMENT },
    { "Mana Storm", 75, MANA_STORM },
    { "Lightning Bolt", 85, LIGHTNING_BOLT },
    { "Fire Bolt", 85, FIRE_BOLT },
    { "Acid Bolt", 85, ACID_BOLT },
    { "Frost Ball", 80, FROST_BALL },
    { "Acid Ball", 80, ACID_BALL },
    { "Cloud Kill", 75, CLOUD_KILL }
};
static borg_spell_rating borg_spell_ratings_DRUID[] =
{
    { "Detect Life", 95,  DETECT_LIFE },
    { "Fox Form", 5, FOX_FORM }, // !FIX !TODO need to know when to cast any of the shapechanges
    { "Remove Hunger", 85, REMOVE_HUNGER },
    { "Stinking Cloud", 95, STINKING_CLOUD },
    { "Confuse Monster", 55, CONFUSE_MONSTER },
    { "Slow Monster", 65, SLOW_MONSTER },
    { "Cure Poison", 55, CURE_POISON },
    { "Resist Poison", 60, RESIST_POISON },
    { "Turn Stone to Mud", 80, TURN_STONE_TO_MUD },
    { "Sense Surroundings", 80, SENSE_SURROUNDINGS },
    { "Lightning Strike", 85, LIGHTNING_STRIKE },
    { "Earth Rising", 70, EARTH_RISING },
    { "Trance", 55, TRANCE },
    { "Mass Sleep", 80, MASS_SLEEP },
    { "Become Pukel-man", 5, BECOME_PUKEL_MAN }, // !FIX !TODO shapechange
    { "Eagle's Flight", 5, EAGLES_FLIGHT }, // !FIX !TODO shapechange
    { "Bear Form", 5, BEAR_FORM }, // !FIX !TODO shapechange
    { "Tremor", 80, TREMOR },
    { "Haste Self", 90, HASTE_SELF },
    { "Revitalize", 95, REVITALIZE },
    { "Rapid Regeneration", 55, RAPID_REGENERATION },
    { "Herbal Curing", 90, HERBAL_CURING },
    { "Meteor Swarm", 90, METEOR_SWARM },
    { "Rift", 90, RIFT },
    { "Ice Storm", 85, ICE_STORM },
    { "Volcanic Eruption", 60, VOLCANIC_ERUPTION },
    { "River of Lightning", 90, RIVER_OF_LIGHTNING }
};

static borg_spell_rating borg_spell_ratings_PRIEST[] =
{
    { "Call Light", 65, CALL_LIGHT },
    { "Detect Evil", 85, DETECT_EVIL },
    { "Minor Healing", 65, MINOR_HEALING },
    { "Cure Serious Wounds", 65, CURE_SERIOUS_WOUNDS },
    { "Cure Critical Wounds", 75, CURE_CRITICAL_WOUNDS },
    { "Cure Mortal Wounds", 85, CURE_MORTAL_WOUNDS },
    { "Bless", 85, BLESS },
    { "Sense Invisible", 75, SENSE_INVISIBLE },
    { "Heroism", 75, HEROISM },
    { "Orb of Draining", 95, ORB_OF_DRAINING },
    { "Spear of Light", 75, SPEAR_OF_LIGHT },
    { "Dispel Undead", 65, DISPEL_UNDEAD },
    { "Dispel Evil", 65, DISPEL_EVIL },
    { "Protection from Evil", 85, PROTECTION_FROM_EVIL },
    { "Remove Curse", 85, REMOVE_CURSE },
    { "Portal", 85, PORTAL },
    { "Remembrance", 75, REMEMBRANCE },
    { "Word of Recall", 95, WORD_OF_RECALL },
    { "Healing", 95, HEALING },
    { "Restoration", 75, RESTORATION },
    { "Clairvoyance", 85, CLAIRVOYANCE },
    { "Enchant Weapon", 75, ENCHANT_WEAPON },
    { "Enchant Armour", 75, ENCHANT_ARMOUR },
    { "Smite Evil", 75, SMITE_EVIL },
    { "Glyph of Warding", 95, GLYPH_OF_WARDING },
    { "Demon Bane", 85, DEMON_BANE },
    { "Banish Evil", 85, BANISH_EVIL },
    { "Word of Destruction", 75, WORD_OF_DESTRUCTION },
    { "Holy Word", 85, HOLY_WORD },
    { "Spear of Orom\xC3\xab", 85, SPEAR_OF_OROME }, /* "Spear of Orom(e + diaresis)" */
    { "Light of Varda", 85, LIGHT_OF_VARDA } /* "Light of Varda"*/
};
static borg_spell_rating borg_spell_ratings_PALADIN[] =
{
    { "Bless", 95, BLESS },
    { "Detect Evil", 85, DETECT_EVIL },
    { "Call Light", 85, CALL_LIGHT },
    { "Minor Healing", 95, MINOR_HEALING },
    { "Cure Serious Wounds", 85, CURE_SERIOUS_WOUNDS },
    { "Cure Critical Wounds", 90, CURE_CRITICAL_WOUNDS },
    { "Cure Mortal Wounds", 95, CURE_MORTAL_WOUNDS },
    { "Sense Invisible", 65, SENSE_INVISIBLE },
    { "Heroism", 85, HEROISM },
    { "Protection from Evil", 85, PROTECTION_FROM_EVIL },
    { "Remove Curse", 65, REMOVE_CURSE },
    { "Word of Recall", 95, WORD_OF_RECALL },
    { "Healing", 95, HEALING },
    { "Clairvoyance", 85, CLAIRVOYANCE },
    { "Smite Evil", 55, SMITE_EVIL },
    { "Demon Bane", 55, DEMON_BANE },
    { "Enchant Weapon", 75, ENCHANT_WEAPON },
    { "Enchant Armour", 85, ENCHANT_ARMOUR },
    { "Single Combat", 95, SINGLE_COMBAT } // !FIX !TODO defense?  not sure how to code.
};
static borg_spell_rating borg_spell_ratings_ROGUE[] =
{
    { "Detect Monsters", 85, DETECT_MONSTERS },
    { "Phase Door", 95, PHASE_DOOR },
    { "Object Detection", 55, OBJECT_DETECTION },
    { "Detect Stairs", 55, DETECT_STAIRS },
    { "Recharging", 85, RECHARGING },
    { "Reveal Monsters", 85, REVEAL_MONSTERS },
    { "Teleport Self", 95, TELEPORT_SELF },
    { "Hit and Run", 15, HIT_AND_RUN }, // !FIX !TODO not sure how to code this
    { "Teleport Other", 85, TELEPORT_OTHER },
    { "Teleport Level", 75, TELEPORT_LEVEL },
    { "Spear of Light", 75, SPEAR_OF_LIGHT },
    { "Heroism", 75, HEROISM },
    { "Berserker", 75, BERSERK_STRENGTH },
    { "Resist Cold", 75, RESIST_COLD_SPELL },
    { "Resist Fire", 75, RESIST_FIRE_SPELL },
    { "Shield", 75, SHIELD_SPELL }
};
static borg_spell_rating borg_spell_ratings_RANGER[] =
{
    { "Remove Hunger", 95, REMOVE_HUNGER },
    { "Detect Life", 85, DETECT_LIFE },
    { "Herbal Curing", 95, HERBAL_CURING },
    { "Resist Poison", 85, RESIST_POISON },
    { "Turn Stone to Mud", 85, TURN_STONE_TO_MUD },
    { "Sense Surroundings", 75, SENSE_SURROUNDINGS },
    { "Cover Tracks", 25, COVER_TRACKS }, // !FIX !TODO prep?
    { "Create Arrows", 85, CREATE_ARROWS }, // !FIX !TODO
    { "Haste Self", 95, HASTE_SELF },
    { "Decoy", 5, DECOY }, // !FIX !TODO not sure what to do with this
    { "Brand Ammunition", 95, BRAND_AMMUNITION },
    { "Spear of Light", 75, SPEAR_OF_LIGHT },
    { "Heroism", 85, HEROISM },
    { "Berserker", 75, BERSERK_STRENGTH },
    { "Resist Cold", 85, RESIST_COLD_SPELL },
    { "Resist Fire", 85, RESIST_FIRE_SPELL },
    { "Shield", 85, SHIELD_SPELL }
};

/*
 * get the stat used for casting spells
 *
 * Assumes the first spell determines the realm thus stat for all spells
 */
int borg_spell_stat(void)
{
    if (borg_can_cast()) {
        struct class_spell *spell = &(player->class->magic.books[0].spells[0]);
        if (spell != NULL) {
            return spell->realm->stat;
        }
    }

    return -1;
}

/*
 * Does this player cast spells
 */
bool borg_can_cast(void)
{
    return player->class->magic.total_spells != 0;
}

/*
 * Does this player mostly cast spells
 * HACK: Rather than hard code classes, assume any class with
 * more than three books is primarily casting
 * !FIX !TODO consider adding is_primary_caster to class struct
 */
bool borg_primarily_caster(void)
{
    return player->class->magic.num_books > 3;
}

/*
 * get the level at which Heroism (spell) grants Heroism (effect)
 * In this fork only Rogues and Rangers have the spell and it always
 * grants heroism.
 */
int borg_heroism_level(void)
{
    return 0;
}

/*
 * find the index in the books array given the books sval
 */
int borg_get_book_num(int sval)
{
    if (!borg_can_cast())
        return -1;

    for (int book_num = 0; book_num < player->class->magic.num_books;
         book_num++) {
        if (player->class->magic.books[book_num].sval == sval)
            return book_num;
    }
    return -1;
}

/*
 * is this a dungeon book (not a basic book)
 */
bool borg_is_dungeon_book(int tval, int sval)
{
    switch (tval) {
    case TV_PRAYER_BOOK:
    case TV_MAGIC_BOOK:
    case TV_NATURE_BOOK:
    case TV_SHADOW_BOOK:
    case TV_OTHER_BOOK:
        break;
    default:
        return false;
    }

    /* keep track of if this is a book from the dungeon */
    for (int i = 0; i < player->class->magic.num_books; i++) {
        struct class_book book = player->class->magic.books[i];
        if (tval == book.tval && sval == book.sval && book.dungeon)
            return true;
    }

    return false;
}

/*
 * Find the magic structure given a book/entry
 */
borg_magic *borg_get_spell_entry(int book, int entry)
{
    int entry_in_book = 0;

    for (int spell_num = 0; spell_num < player->class->magic.total_spells;
         spell_num++) {
        if (borg_magics[spell_num].book == book) {
            if (entry_in_book == entry)
                return &borg_magics[spell_num];
            entry_in_book++;
        }
    }
    return NULL;
}

/*
 * Find the spell index for a given spell
 */
static int borg_get_spell_number(const enum borg_spells spell)
{
    int first_match = -1;

    /* The borg must be able to "cast" spells */
    if (borg_magics == NULL)
        return -1;

    int total_spells = player->class->magic.total_spells;
    for (int spell_num = 0; spell_num < total_spells; spell_num++) {
        borg_magic *as = &borg_magics[spell_num];

        if (as->spell_enum != spell)
            continue;

        if (first_match < 0)
            first_match = spell_num;

        if (borg.book_idx[as->book] >= 0
            && as->status >= BORG_MAGIC_TEST)
            return spell_num;
    }

    return first_match;
}

/*
 * Find the power (cost in sp) value for a given spell
 */
int borg_get_spell_power(const enum borg_spells spell)
{
    int spell_num = borg_get_spell_number(spell);
    if (spell_num < 0)
        return -1;

    borg_magic *as = &borg_magics[spell_num];

    return as->power;
}

/*
 * Determine if borg can cast a given spell (when fully rested)
 */
static bool borg_spell_entry_legal(int spell_num)
{
    if (spell_num < 0)
        return false;

    borg_magic *as = &borg_magics[spell_num];

    /* The book must be possessed */
    if (borg.book_idx[as->book] < 0)
        return false;

    /* The spell must be "known" */
    if (borg_magics[spell_num].status < BORG_MAGIC_TEST)
        return false;

    /* The spell must be affordable (when rested) */
    if (borg_magics[spell_num].power > borg.trait[BI_MAXSP])
        return false;

    /* Success */
    return true;
}

bool borg_spell_legal(const enum borg_spells spell)
{
    return borg_spell_entry_legal(borg_get_spell_number(spell));
}

/*
 * check a spell for a given effect
 */
static bool borg_spell_has_effect(int spell_num, uint16_t effect)
{
    const struct class_spell *cspell = spell_by_index(player, spell_num);
    struct effect            *eff    = cspell->effect;
    while (eff != NULL) {
        if (eff->index == effect)
            return true;
        eff = eff->next;
    }
    return false;
}

/*
 * Get the mana reserve used by spell availability and attack scoring.
 */
int borg_get_mana_reserve(void)
{
    int reserve_mana = 0;
    int teleport_spell_num;

    switch (borg.trait[BI_CLASS]) {
    case CLASS_MAGE:
        reserve_mana = 6;
        break;
    case CLASS_RANGER:
        reserve_mana = 22;
        break;
    case CLASS_ROGUE:
        reserve_mana = 20;
        break;
    case CLASS_PRIEST:
        reserve_mana = 8;
        break;
    case CLASS_PALADIN:
        reserve_mana = 20;
        break;
    }

    /* Low-level spellcasters should not reserve mana yet. */
    if (borg.trait[BI_CLEVEL] < 35)
        return 0;

    /* Prefer the live cost when Teleport Self is available. */
    teleport_spell_num = borg_get_spell_number(TELEPORT_SELF);
    if (borg_spell_entry_legal(teleport_spell_num))
        return borg_magics[teleport_spell_num].power;

    return reserve_mana;
}

/*
 * Determine if borg can cast a given spell (right now)
 */
bool borg_spell_okay(const enum borg_spells spell)
{
    int spell_num    = borg_get_spell_number(spell);
    if (spell_num < 0)
        return false;

    borg_magic *as = &borg_magics[spell_num];

    /* Dark */
    if (no_light(player))
        return false;

    int reserve_mana = borg_get_mana_reserve();

    /* Require ability (when rested) */
    if (!borg_spell_entry_legal(spell_num))
        return false;

    /* Blind/confused/amnesia */
    if (borg.trait[BI_ISBLIND] || borg.trait[BI_ISCONFUSED])
        return false;

    /* The spell must be affordable (now) */
    if (as->power > borg.trait[BI_CURSP])
        return false;

    /* Do not cut into reserve mana (for final teleport) */
    if (borg.trait[BI_CURSP] - as->power < reserve_mana) {
        /* nourishing spells okay */
        if (borg_spell_has_effect(spell_num, EF_NOURISH))
            return true;

        /* okay to run away */
        if (borg_spell_has_effect(spell_num, EF_TELEPORT))
            return true;

        /* Magic Missile OK */
        if (MAGIC_MISSILE == spell && borg.trait[BI_CDEPTH] <= 35)
            return true;

        /* others are rejected */
        return false;
    }

    /* Success */
    return true;
}

/*
 * fail rate on a spell
 */
int borg_spell_fail_rate(const enum borg_spells spell)
{
    int chance, minfail;

    int spell_num = borg_get_spell_number(spell);
    if (spell_num < 0)
        return 100;

    borg_magic *as = &borg_magics[spell_num];

    /* Access the spell  */
    chance = as->sfail;

    /* Reduce failure rate by "effective" level adjustment */
    chance -= 3 * (borg.trait[BI_CLEVEL] - as->level);

    /* Reduce failure rate by stat adjustment */
    chance -= borg.trait[BI_FAIL1];

    /* Fear makes the failrate higher */
    if (borg.trait[BI_ISAFRAID])
        chance += 20;

    /* Extract the minimum failure rate */
    minfail = borg.trait[BI_FAIL2];

    /* Non mage characters never get too good */
    if (!player_has(player, PF_ZERO_FAIL)) {
        if (minfail < 5)
            minfail = 5;
    }

    /* Necromancers are punished by being on lit squares */
    /* necromancers like the dark */
    if (borg.trait[BI_CLASS] == CLASS_NECROMANCER &&
        borg_grids[borg.c.y][borg.c.x].info & BORG_LIGHT) {
        chance += 25;

    }

    /* Minimum failure rate and max */
    if (chance < minfail)
        chance = minfail;
    if (chance > 50)
        chance = 50;

    /* Stunning makes spells harder */
    if (borg.trait[BI_ISHEAVYSTUN])
        chance += 25;
    if (borg.trait[BI_ISSTUN])
        chance += 15;

    /* Amnesia makes it harder */
    if (borg.trait[BI_ISFORGET])
        chance *= 2;

    /* Always a 5 percent chance of working */
    if (chance > 95)
        chance = 95;

    /* Return the chance */
    return (chance);
}

/*
 * same as borg_spell_okay with a fail % check
 */
bool borg_spell_okay_fail(const enum borg_spells spell, int allow_fail)
{
    if (borg_spell_fail_rate(spell) > allow_fail)
        return false;
    return borg_spell_okay(spell);
}

/*
 * Same as borg_spell with a fail % check
 */
bool borg_spell_fail(const enum borg_spells spell, int allow_fail)
{
    if (borg_spell_fail_rate(spell) > allow_fail)
        return false;
    return borg_spell(spell);
}

/*
 * Same as borg_spell_legal with a fail % check
 */
bool borg_spell_legal_fail(const enum borg_spells spell, int allow_fail)
{
    if (borg_spell_fail_rate(spell) > allow_fail)
        return false;
    return borg_spell_legal(spell);
}

/*
 * Attempt to cast a spell
 */
bool borg_spell(const enum borg_spells spell)
{
    int i;

    int spell_num = borg_get_spell_number(spell);
    if (spell_num < 0)
        return false;

    borg_magic *as = &borg_magics[spell_num];

    /* Require ability (right now) */
    if (!borg_spell_okay(spell))
        return false;

    /* Look for the book */
    i = borg.book_idx[as->book];

    /* Paranoia */
    if (i < 0)
        return false;

    /* Debugging Info */
    borg_note(format("# Casting %s (%d,%d).", as->name, i, as->book_offset));

    /* Cast a spell */
    borg_keypress('m');
    borg_keypress(all_letters_nohjkl[i]);
    borg_keypress(all_letters_nohjkl[as->book_offset]);

    /* increment the spell counter */
    as->times++;

    /* Success */
    return true;
}

/*
 * Cheat the "spell" info for a single book
 */
static void borg_cheat_spell(int book_num)
{
    struct class_book *book = &player->class->magic.books[book_num];
    for (int spell_num = 0; spell_num < book->num_spells; spell_num++) {
        struct class_spell *cspell = &book->spells[spell_num];
        borg_magic *as = &borg_magics[cspell->sidx];

        /* Note "forgotten" spells */
        if (player->spell_flags[cspell->sidx] & PY_SPELL_FORGOTTEN) {
            /* Forgotten */
            as->status = BORG_MAGIC_LOST;
        }

        /* Note "difficult" spells */
        else if (borg.trait[BI_CLEVEL] < as->level) {
            /* Unknown */
            as->status = BORG_MAGIC_HIGH;
        }

        /* Note "Unknown" spells */
        else if (!(player->spell_flags[cspell->sidx] & PY_SPELL_LEARNED)) {
            /* UnKnown */
            as->status = BORG_MAGIC_OKAY;
        }

        /* Note "untried" spells */
        else if (!(player->spell_flags[cspell->sidx] & PY_SPELL_WORKED)) {
            /* Untried */
            as->status = BORG_MAGIC_TEST;
        }

        /* Note "known" spells */
        else {
            /* Known */
            as->status = BORG_MAGIC_KNOW;
        }
    }
}

/*
 * Cheat the "spell" info
 */
void borg_cheat_spells(void)
{
    int i;

    /* Assume no books */
    for (i = 0; i < 9; i++)
        borg.book_idx[i] = -1;

    /* Scan the pack */
    for (i = 0; i < z_info->pack_size; i++) {
        int        book_num;
        borg_item *item = &borg_items[i];

        for (book_num = 0; book_num < player->class->magic.num_books;
            book_num++) {
            struct class_book book = player->class->magic.books[book_num];
            if (item->tval == book.tval && item->sval == book.sval) {
                /* Note book locations */
                borg.book_idx[book_num] = i;
                break;
            }
        }
    }

    /* only browse spells if casting is possible */
    if (!borg_can_cast())
        return;

    /* XXX XXX XXX Dark */

    for (int book_idx = 0; book_idx < 8; book_idx++)         {
        /* Look for the book */
        i = borg.book_idx[book_idx];

        /* Cheat the "spell" screens (all of them) */
        if (i >= 0)
            /* Cheat that page */
            borg_cheat_spell(book_idx);
    }

    return;
}


/*
 * Get the offset in the book this spell is so you can cast it (book) (offset)
 */
static int borg_get_book_offset(int index)
{
    int                       book = 0, count = 0;
    const struct class_magic *magic = &player->class->magic;

    /* Check index validity */
    if (index < 0 || index >= magic->total_spells)
        return -1;

    /* Find the book, count the spells in previous books */
    while (count + magic->books[book].num_spells - 1 < index)
        count += magic->books[book++].num_spells;

    /* Find the spell */
    return index - count;
}

/*
 * Some spells have a different name in this fork than the one the borg
 * knows them by.
 */
static const struct {
    const char *name;
    const char *borg_name;
} borg_spell_aliases[] = {
    { "Light Area", "Light Room" },
    { "Lesser Recharging", "Recharging" },
    { "Greater Recharging", "Recharging" },
    { "Enchant Armor", "Enchant Armour" },
    { "Cure Light Wounds", "Minor Healing" },
    { "Heal", "Healing" },
    { "Blink", "Phase Door" }
};

static struct {
    const borg_spell_rating *ratings;
    size_t                   count;
} borg_rating_tables[4];
static size_t borg_rating_table_count;

static void borg_add_rating_table(
    size_t *n, const borg_spell_rating *ratings, size_t count)
{
    borg_rating_tables[*n].ratings = ratings;
    borg_rating_tables[*n].count   = count;
    (*n)++;
}

/*
 * Find the borg's rating for a spell by name.  Tables are searched in the
 * order they were added so the class's own table wins.
 */
static const borg_spell_rating *borg_find_spell_rating(const char *name)
{
    for (size_t a = 0; a < N_ELEMENTS(borg_spell_aliases); a++) {
        if (streq(name, borg_spell_aliases[a].name)) {
            name = borg_spell_aliases[a].borg_name;
            break;
        }
    }

    for (size_t t = 0; t < borg_rating_table_count; t++) {
        for (size_t i = 0; i < borg_rating_tables[t].count; i++) {
            if (streq(name, borg_rating_tables[t].ratings[i].name))
                return &borg_rating_tables[t].ratings[i];
        }
    }
    return NULL;
}

/*
 * initialize the spell data
 */
static void borg_init_spell(borg_magic *spells, int spell_num)
{
    borg_magic               *spell  = &spells[spell_num];
    const struct class_spell *cspell = spell_by_index(player, spell_num);
    const borg_spell_rating  *rating = borg_find_spell_rating(cspell->name);

    /* Spells the borg has no code for get no rating and are never cast */
    spell->rating       = rating ? rating->rating : 0;
    spell->name         = cspell->name;
    spell->spell_enum   = rating ? rating->spell_enum : NO_SPELL;
    spell->level        = cspell->slevel;
    spell->book_offset  = borg_get_book_offset(cspell->sidx);
    spell->effect_index = cspell->effect->index;
    spell->power        = cspell->smana;
    spell->sfail        = cspell->sfail;
    spell->status       = spell_okay_to_cast(player, spell_num);
    spell->times        = 0;
    spell->book         = cspell->bidx;
}

/*
 * Prepare a book
 */
void borg_prepare_book_info(void)
{
    size_t n = 0;

    switch (player->class->cidx) {
    case CLASS_MAGE:
        borg_add_rating_table(&n, borg_spell_ratings_MAGE,
            N_ELEMENTS(borg_spell_ratings_MAGE));
        borg_add_rating_table(&n, borg_spell_ratings_DRUID,
            N_ELEMENTS(borg_spell_ratings_DRUID));
        break;
    case CLASS_ROGUE:
        borg_add_rating_table(&n, borg_spell_ratings_ROGUE,
            N_ELEMENTS(borg_spell_ratings_ROGUE));
        borg_add_rating_table(&n, borg_spell_ratings_MAGE,
            N_ELEMENTS(borg_spell_ratings_MAGE));
        borg_add_rating_table(&n, borg_spell_ratings_DRUID,
            N_ELEMENTS(borg_spell_ratings_DRUID));
        break;
    case CLASS_RANGER:
        borg_add_rating_table(&n, borg_spell_ratings_RANGER,
            N_ELEMENTS(borg_spell_ratings_RANGER));
        borg_add_rating_table(&n, borg_spell_ratings_MAGE,
            N_ELEMENTS(borg_spell_ratings_MAGE));
        borg_add_rating_table(&n, borg_spell_ratings_DRUID,
            N_ELEMENTS(borg_spell_ratings_DRUID));
        break;
    case CLASS_PRIEST:
        borg_add_rating_table(&n, borg_spell_ratings_PRIEST,
            N_ELEMENTS(borg_spell_ratings_PRIEST));
        borg_add_rating_table(&n, borg_spell_ratings_PALADIN,
            N_ELEMENTS(borg_spell_ratings_PALADIN));
        borg_add_rating_table(&n, borg_spell_ratings_MAGE,
            N_ELEMENTS(borg_spell_ratings_MAGE));
        borg_add_rating_table(&n, borg_spell_ratings_DRUID,
            N_ELEMENTS(borg_spell_ratings_DRUID));
        break;
    case CLASS_PALADIN:
        borg_add_rating_table(&n, borg_spell_ratings_PALADIN,
            N_ELEMENTS(borg_spell_ratings_PALADIN));
        borg_add_rating_table(&n, borg_spell_ratings_PRIEST,
            N_ELEMENTS(borg_spell_ratings_PRIEST));
        borg_add_rating_table(&n, borg_spell_ratings_MAGE,
            N_ELEMENTS(borg_spell_ratings_MAGE));
        borg_add_rating_table(&n, borg_spell_ratings_DRUID,
            N_ELEMENTS(borg_spell_ratings_DRUID));
        break;
    default:
        break;
    }
    borg_rating_table_count = n;

    if (borg_magics) {
        mem_free(borg_magics);
        borg_magics = NULL;
    }

    if (player->class->magic.total_spells == 0)
        return;

    borg_magics
        = mem_zalloc(player->class->magic.total_spells * sizeof(borg_magic));

    for (int spell = 0; spell < player->class->magic.total_spells; spell++) {
        borg_init_spell(borg_magics, spell);
    }
}

#endif

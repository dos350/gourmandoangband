# Gourmand class notes

This file records the current Gourmand design and where its behavior is
implemented. The game-data class record is in `lib/gamedata/class.txt`.

## Current mechanics

- The class has a base speed adjustment of -3. That class penalty is removed
  while the food status is `Fed` or `Full`; other speed modifiers still apply.
- Gourmands digest food twice as quickly, including while `Full`. Below the
  `Hungry` threshold they take periodic hunger damage, with more damage below
  `Starving`.
- After successfully eating a `TV_FOOD` item whose subtype value is higher
  than Ration of Food's, a Gourmand gets one random benefit: 3-8 healing, 11-20
  turns of speed, or clearing one active harmful timed effect. If there is no
  effect to clear, that result gives 3-8 healing instead. Slime Mold and Ration
  of Food do not qualify. The subtype comparison follows the food-kind ordering
  in `lib/gamedata/object.txt`.
- At level 5, press **N** to sniff out visible monsters up to 8 tiles away.
  Range increases by 1 tile for every 2 levels above level 5. This is a class
  ability, not a spell, and uses a turn. **V** remains reserved for version
  information.

## Implementation map

- `lib/gamedata/class.txt`: class stats, base speed, starting equipment, and
  `GOURMAND` player flag.
- `lib/gamedata/player_property.txt`: player-facing description of the
  Gourmand flag, metabolism, hunger, and monster-sensing ability.
- `src/init.c` and `src/player.h`: parse and store the class `speed` value.
- `src/player-calcs.c`: apply class speed and remove the penalty when Fed or
  Full.
- `src/game-world.c`: faster digestion and periodic hunger damage.
- `src/cmd-obj.c`: food bonuses and the monster-sensing command.
- `src/cmd-core.c`, `src/cmd-core.h`, `src/cmds.h`, `src/player-util.c`,
  `src/player-util.h`, and `src/ui-game.c`: command registration, level
  prerequisite, and **N** key binding.
- `src/list-player-flags.h`: declaration of the `GOURMAND` player flag.

## Parser note

The `desc:` field in `class.txt` is for the most recently defined spell, not a
general class description. Adding `desc:` lines after a non-spellcasting class
therefore causes a parse error. Keep general class-ability descriptions in
`lib/gamedata/player_property.txt` or this note.

## Verification note

The class parser was the source of a reported error when general descriptions
were added to `class.txt`; those invalid lines were removed. A whitespace/diff
check passed afterward. The Windows build task was not available through the
task runner during that check, so a full build was not confirmed.

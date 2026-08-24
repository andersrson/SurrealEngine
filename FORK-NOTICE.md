# Fork notice

**This is an altered version of Surreal Engine. It is not the original software.**

Upstream: https://github.com/dpjudas/SurrealEngine
Original authors: Magnus Norddahl, Lupert Everett and contributors.

This fork is maintained independently and is **not affiliated with, endorsed by, or supported by**
the upstream project. Do not report problems found in this fork to upstream, and do not report
problems found in upstream here.

## What differs

This fork narrows the focus to **Deus Ex (2000)**, targeting the Steam GOTY release (v1.112fm), and
adds Deus Ex specific engine work that upstream does not carry. Changes to upstream source are
marked in-line with `// [DXFORK]`, so the full delta can be found with a single grep.

## Why it is a fork rather than a contribution

Upstream asks that changes made primarily with LLM assistance not be submitted to them, and be kept
in a fork instead (see `NO-AI Code Rule.md`). This fork honours that. Contributions that are
hand-authored and generally useful are sent upstream separately as ordinary pull requests.

## Licensing

Surreal Engine is distributed under the zlib license, reproduced in full in `LICENSE.md`, which
this fork retains unaltered. Two of its conditions bear directly on this file:

> 1. The origin of this software must not be misrepresented; you must not claim that you wrote the
>    original software. …
> 2. Altered source versions must be plainly marked as such, and must not be misrepresented as
>    being the original software.

This notice exists to satisfy condition 2. Condition 1's suggested acknowledgement is given above
and, where this fork ships a binary, is repeated in its About dialog.

Note that `SurrealVideo` is licensed separately under the LGPL 2.1 and must remain a dynamically
linked shared library in any distributed build. See `LICENSE.md` for the licences of all bundled
third-party components.

## Game data

No Deus Ex game data is included or redistributed. This software requires a separately obtained,
legitimately licensed copy of the game.

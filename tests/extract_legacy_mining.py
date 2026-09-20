"""Test the mining statement from the generated game, not a copied formula."""
from pathlib import Path
import re
import sys

source = Path(sys.argv[1])
statements = re.findall(r'float cm\s*=\s*[^;]+;', source.read_text())
if len(statements) != 1:
    raise SystemExit('Expected exactly one legacy mining statement')
(source.parent / 'legacy-mining-snippet.hpp').write_text(
    'static float sfTestMiningStep() { ' + statements[0] + ' return cm; }\n')

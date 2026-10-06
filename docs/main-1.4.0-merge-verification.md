# Vérification `main` après fusion 1.4.0

Ce commit déclenche la vérification Android fraîche de `main` après :

- publication publique de `v1.4.0` depuis `98da8a01175d5091f487232f65872e92d213b321` ;
- merge à deux parents `c50088092744d18ce50f3ef484a2d706aa76a276` ;
- synchronisation des quatre mémoires vivantes avec la release et le merge.

La clôture n'est acquise qu'après succès des régressions, du build Gradle et du packaging sur ce HEAD.

# 🪵 WoodPilot - Gestion Des Étapes De Fabrication

Application Qt Creator C++ pour gérer les **étapes de fabrication** de produits en bois avec un thème couleurs naturelles.

## 🎨 Thème Couleurs de Bois

L'interface utilise des couleurs inspirées du bois naturel:
- **Marron clair** (#A0826D) - Chêne
- **Marron moyen** (#8B6F47) - Noyer  
- **Marron foncé** (#6B5644) - Acajou
- **Beige** (#F5EFE7) - Pin
- **Sable** (#D2B48C) - Bouleau

## ✨ Fonctionnalités

### Gestion Complète des Étapes
- ✅ **Nom de l'étape** (ex: "Découpage de bois")
- ✅ **Produit concerné** (Table, Chaise, Armoire, etc.)
- ✅ **Numéro/Ordre** (1, 2, 3...)
- ✅ **Temps estimé** (en minutes)
- ✅ **Description détaillée** (zone de texte multiligne)
- ✅ **Date de début** (calendrier)
- ✅ **Date de fin** (calendrier)
- ✅ **État** (Non commencé / En cours / Terminé)

### Opérations CRUD
- ➕ **CREATE** - Ajouter une nouvelle étape
- 📖 **READ** - Voir toutes les étapes
- ✏️ **UPDATE** - Modifier une étape existante
- 🗑️ **DELETE** - Supprimer une étape

### Fonctionnalités Avancées
- 🔍 **Recherche** - Par nom, produit ou description
- 🎯 **Filtre par produit** - Voir uniquement les étapes d'un produit
- 📊 **Tri automatique** - Par produit puis par ordre
- 📅 **Dates avec calendrier** - Sélection facile des dates
- 🎨 **Interface bois** - Thème professionnel

## 📁 Structure du Projet

```
WoodPilot_Etapes/
├── main.cpp              - Point d'entrée
├── MainWindow.h/cpp      - Interface principale
├── Etape.h/cpp           - Modèle de données Étape
├── DatabaseManager.h/cpp - Gestion base de données
├── StyleSheet.h          - Thème couleurs bois
└── WoodPilot_Etapes.pro  - Fichier projet Qt
```

## 🚀 Installation

### 1️⃣ Télécharger les Fichiers
Téléchargez TOUS les fichiers dans un dossier "WoodPilot"

### 2️⃣ Ouvrir dans Qt Creator
```
Qt Creator → Fichier → Ouvrir un fichier ou projet
Sélectionnez: WoodPilot_Etapes.pro
Configurez le projet avec votre kit Qt
```

### 3️⃣ Compiler et Exécuter
```
Cliquez sur 🔨 (Compiler) ou Ctrl+B
Cliquez sur ▶️ (Exécuter) ou Ctrl+R
```

## 📝 Exemple d'Utilisation

### Pour une TABLE:
```
Étape 1: Découpage de bois
- Produit: Table
- Ordre: 1
- Temps: 60 minutes
- État: Non commencé
- Description: Découper les planches selon le plan

Étape 2: Montage
- Produit: Table
- Ordre: 2
- Temps: 90 minutes
- État: Non commencé
- Description: Assembler les planches avec vis et colle

Étape 3: Peinture
- Produit: Table
- Ordre: 3
- Temps: 45 minutes
- État: Non commencé
- Description: Appliquer 2 couches de vernis

Étape 4: Séchage
- Produit: Table
- Ordre: 4
- Temps: 1440 minutes (24h)
- État: Non commencé
- Description: Laisser sécher à température ambiante
```

## 🎯 Guide d'Utilisation

### Ajouter une Étape
1. Remplir le formulaire (gauche):
   - Nom de l'étape (obligatoire)
   - Sélectionner le produit
   - Entrer l'ordre (1, 2, 3...)
   - Temps estimé en minutes
   - Choisir les dates
   - Sélectionner l'état
   - Ajouter une description
2. Cliquer **"Ajouter"**
3. L'étape apparaît dans le tableau

### Modifier une Étape
1. Cliquer sur une ligne du tableau
2. Les champs se remplissent automatiquement
3. Modifier les informations
4. Cliquer **"Modifier"**

### Supprimer une Étape
1. Sélectionner une ligne
2. Cliquer **"Supprimer"**
3. Confirmer

### Rechercher
1. Taper dans "Recherche"
2. Cliquer **"Chercher"**
3. Résultats filtrés s'affichent

### Filtrer par Produit
1. Choisir un produit dans le menu déroulant
2. Cliquer **"Filtrer"**
3. Seules les étapes de ce produit s'affichent

## 🗄️ Base de Données

**Fichier**: `etapes_fabrication.db`  
**Table**: `etapes`

```sql
CREATE TABLE etapes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nom TEXT NOT NULL,
    produit TEXT NOT NULL,
    ordre INTEGER,
    temps_estime INTEGER,
    description TEXT,
    date_debut TEXT,
    date_fin TEXT,
    etat TEXT
);
```

## 🎨 Couleurs du Thème

| Élément | Couleur | Ton de Bois |
|---------|---------|-------------|
| Fond principal | #F5EFE7 | Pin clair |
| Onglets actifs | #A0826D | Chêne moyen |
| Onglets inactifs | #6B5644 | Noyer foncé |
| Bordures | #C4A57B | Bouleau |
| Tableaux alternés | #DEB887 | Sable |
| Boutons | #6B5644 → #8B6F47 | Bois naturel |

## 🔧 Personnalisation

### Ajouter un Produit
Dans `MainWindow.cpp`, ligne ~90:
```cpp
produitCombo->addItems({"Table", "Chaise", "Armoire", 
                        "VOTRE_NOUVEAU_PRODUIT"});
```

### Changer les Couleurs
Dans `StyleSheet.h`, modifiez les codes hexadécimaux:
```cpp
background-color: #A0826D;  // Changer cette couleur
```

### Ajouter un État
Dans `MainWindow.cpp`, ligne ~110:
```cpp
etatCombo->addItems({"Non commencé", "En cours", "Terminé", 
                     "VOTRE_NOUVEL_ETAT"});
```

## 📊 Onglets

### 1. Gestion Des Étapes
- Formulaire complet de saisie
- Tableau avec toutes les étapes
- Recherche et filtres
- Actions CRUD

### 2. Statistiques
- Zone réservée pour:
  - Graphiques de progression
  - Temps total par produit
  - Étapes en retard
  - Analyses de performance

## 🎓 Pour Votre Présentation

### Points Forts:
✅ Interface professionnelle thème bois  
✅ Gestion complète des étapes de fabrication  
✅ Tous les champs demandés (8 champs)  
✅ Base de données SQLite persistante  
✅ Recherche et filtrage avancés  
✅ Interface française complète  
✅ Design moderne et intuitif  

### Démonstration:
1. Montrer l'ajout d'une étape "Découpage"
2. Ajouter plusieurs étapes pour une "Table"
3. Filtrer par produit "Table"
4. Modifier l'état en "En cours"
5. Rechercher par mot-clé
6. Montrer le tri automatique

## 💡 Exemples de Produits et Étapes

### CHAISE
1. Découpe des pieds (30 min)
2. Découpe de l'assise (20 min)
3. Découpe du dossier (25 min)
4. Ponçage (40 min)
5. Assemblage (45 min)
6. Vernissage (30 min)
7. Séchage (720 min)

### ARMOIRE
1. Découpe des panneaux (120 min)
2. Découpe des étagères (60 min)
3. Perçage des trous (45 min)
4. Ponçage (90 min)
5. Montage structure (180 min)
6. Installation portes (60 min)
7. Finitions (45 min)
8. Peinture (90 min)
9. Séchage (1440 min)

## ⚠️ Dépannage

| Problème | Solution |
|----------|----------|
| Ne compile pas | Build → Clean All → Rebuild All |
| Base de données erreur | Vérifiez `QT += sql` dans .pro |
| Dates ne s'affichent pas | Vérifiez le format dd/MM/yyyy |
| Thème pas appliqué | Relancez l'application |

## 🔄 Intégration dans Votre Projet

Si vous avez déjà un projet WoodPilot:

1. Copiez les fichiers dans votre projet
2. Ajoutez-les au .pro existant
3. Intégrez l'onglet "Gestion Des Étapes" dans votre TabWidget
4. Adaptez le StyleSheet si nécessaire
5. Modifiez les produits selon votre liste

## ✅ Checklist Finale

- [ ] Application compile sans erreurs
- [ ] Base de données fonctionne
- [ ] Tous les champs sont présents (8 champs)
- [ ] Thème bois s'affiche correctement
- [ ] CRUD fonctionne (Ajouter, Modifier, Supprimer)
- [ ] Recherche fonctionne
- [ ] Filtre par produit fonctionne
- [ ] Dates avec calendrier fonctionnent
- [ ] Ajout de données de test
- [ ] Interface en français

## 🎯 Prêt pour Vendredi!

Votre application de gestion des étapes de fabrication est complète avec:
- Thème couleurs bois professionnel
- Tous les champs demandés
- Interface intuitive et moderne
- Base de données SQLite
- Fonctionnalités CRUD complètes

**Bon courage pour votre projet! 🪵🎓**

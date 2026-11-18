# INF1633 — Programmation de systèmes embarqués en C/C++

## Automne 2026

## PROJET DE FIN DE SESSION — [50 %]

### LECTURE DES DONNÉES DE CAPTEURS SUR ÉCRAN LCD VIA LA CARTE KL27Z

**Délai de remise :** Lundi 08 décembre 2026 à 8 h 00
**Présentations :** Jeudi 10 décembre 2026 (20 min de présentation + 10 min de questions)

---

### 👥 Membres de l'équipe

- **Antoine Mailhot**
- **William Lamoureux**
- **Micaël Racine-Leblanc**

---

### 📝 Titre du projet

**Groupe 1 : Idée 1 et idée 7 — Projet unifié : Station de surveillance environnementale (Température, Pression et CO₂)**

---

### 🎯 Objectif général

Développer un programme en langage C/C++ sur la carte NXP FRDM-KL27Z (microcontrôleur MKL27Z64VLH4) permettant de surveiller différentes conditions environnementales.

Le système utilise notamment :

- Le capteur BMP280 pour mesurer la température et la pression atmosphérique via le bus I²C.
- Le module DFRobot Gravity Analog CO₂ Gas Sensor V1.2 (SEN0159), intégrant une sonde MG-811, pour mesurer le CO₂ au moyen d'une entrée analogique ADC.
- Un écran LCD pour l'affichage local des mesures.
- Une console série UART pour la transmission des données.
- Des DEL pour signaler le fonctionnement normal et les situations d'alerte.

L'architecture logicielle doit permettre de prendre en charge différents capteurs avec des pilotes distincts, sans rendre la logique applicative dépendante d'un modèle particulier.

---

### 🛠️ Matériel et capteurs (configuration par défaut)

- **Microcontrôleur :** Carte NXP FRDM-KL27Z (`MKL27Z64VLH4`).

- **Capteur de température et de pression :** BMP280.
  - Interface : bus I²C.
  - Mesures : température en °C et pression atmosphérique en hPa.

- **Capteur de CO₂ principal (module par défaut) :** DFRobot Gravity — Analog CO₂ Gas Sensor V1.2 (**SKU : SEN0159**, sonde MG-811).
  - **Interface :** sortie analogique (ADC), et non I²C.
  - **Connecteur Gravity à trois broches :**
    - Jaune : signal analogique `A`.
    - Rouge : alimentation `VCC` de 5 V.
    - Noir : masse `GND`.
  - **Plage de mesure annoncée :** 0 à 10 000 ppm.
  - **Seuil d'alerte :** 1000 ppm pour l'activation de la DEL rouge.
  - **Alimentation :** 5 V DC.
  - Le module intègre un circuit élévateur de tension fournissant environ 6 V au chauffage de la sonde.
  - **Consommation du chauffage :** environ 200 mA.

- **Comportement du signal analogique MG-811 :**
  - La tension de sortie analogique diminue lorsque la concentration de CO₂ augmente.
  - La valeur ADC brute ne représente pas directement une concentration en ppm.
  - Une conversion en ppm nécessite une caractérisation et une calibration adaptées au capteur.

- **Sécurité électrique du SEN0159 :**
  - La sortie analogique pouvant atteindre 5 V, elle ne doit jamais être reliée directement à une entrée ADC de la FRDM-KL27Z.
  - Un pont diviseur de tension doit être installé entre la sortie analogique du module et l'entrée ADC.
  - La tension appliquée à l'entrée ADC doit respecter les limites électriques du microcontrôleur, notamment la limite de 3,3 V retenue pour cette interface.
  - Les valeurs des résistances du pont diviseur doivent être calculées et validées avant le branchement.
  - La capacité de l'alimentation 5 V de la carte doit être vérifiée pour supporter la consommation du module.

- **Capteur secondaire optionnel :** Module générique MQ-135 (*Flying-Fish*).
  - Module bleu à quatre broches : `AO`, `DO`, `GND` et `VCC`.
  - `AO` : sortie analogique exploitable par une entrée ADC.
  - `DO` : sortie numérique indiquant le dépassement du seuil configuré sur le module.
  - Utilisation prévue : surveillance complémentaire de la qualité de l'air.
  - Ce module ne remplace pas automatiquement le SEN0159 pour une mesure spécifique du CO₂.

---

### 🔍 Alignement avec les exigences générales du cours

- **Utilisation de la carte KL27Z :** le microcontrôleur MKL27Z64VLH4 orchestre le système.
- **Communication avec les capteurs :** utilisation du bus I²C pour le BMP280 et des entrées ADC pour le SEN0159 et, éventuellement, le MQ-135.
- **Affichage LCD :** présentation locale des mesures environnementales.
- **Communication série :** transmission des données par UART.
- **Indicateurs visuels :** utilisation des DEL verte et rouge pour indiquer l'état du système et les alarmes.
- **Horloge interne :** utilisation des compteurs et mécanismes temporels du microcontrôleur pour gérer les cycles d'acquisition et l'horodatage.

**Note concernant l'exigence de deux capteurs I²C :** le BMP280 utilise I²C, tandis que le SEN0159 utilise une sortie analogique ADC. Le matériel actuellement confirmé ne fournit donc qu'un seul capteur I²C. Si l'exigence pédagogique impose strictement deux capteurs I²C, un deuxième capteur compatible devra être ajouté ou une validation devra être obtenue auprès de l'enseignant.

---

### ⚙️ Fonctionnalités principales

#### 1. Acquisition des données

Le système prévoit un cycle d'acquisition nominal toutes les cinq secondes.

- **BMP280 :**
  - Lecture de la température en °C.
  - Lecture de la pression atmosphérique en hPa.
  - Communication par I²C.

- **SEN0159 / MG-811 :**
  - Acquisition de la valeur brute du signal analogique par ADC.
  - Conversion éventuelle de la valeur brute en ppm dans une couche logicielle distincte.
  - Application d'une calibration appropriée avant d'utiliser les valeurs en ppm pour les alertes.

- **MQ-135 (optionnel) :**
  - Acquisition analogique par ADC ou lecture numérique de la sortie DO.
  - Traitement distinct de celui du SEN0159.

#### 2. Affichage des mesures

- **Local :** affichage des données environnementales sur l'écran LCD.
- **À distance :** transmission et mise en forme des données sur la console série UART.

#### 3. Indicateurs visuels et alertes

- **Fonctionnement normal :** clignotement de la DEL verte à chaque cycle de lecture réussi, nominalement toutes les cinq secondes.
- **Alerte température :** activation de la DEL rouge si la température dépasse 30 °C.
- **Alerte CO₂ :** activation de la DEL rouge si la concentration mesurée dépasse 1000 ppm.
- **Gestion des erreurs :** possibilité de signaler une erreur d'acquisition ou une défaillance d'initialisation d'un capteur.

---

### 🧩 Architecture logicielle et pilotes

Le logiciel doit être organisé en modules indépendants afin de faciliter l'ajout, le remplacement et la maintenance des capteurs.

Les responsabilités prévues sont les suivantes :

- **Pilote MG-811 (`sensor_mg811.c/.h`) :** acquisition analogique brute de référence pour la sonde MG-811.
- **Pilote SEN0159 (`sensor_sen0159.c/.h`) :** gestion du module DFRobot Gravity utilisé comme capteur de CO₂ par défaut.
- **Pilote MQ-135 (`sensor_mq135.c/.h`) :** gestion facultative du module de qualité de l'air.
- **Pilote BMP280 (`sensor_bmp280.c/.h`) :** communication I²C et acquisition de la température et de la pression.
- **Couche applicative :** orchestration des acquisitions, gestion des seuils, affichage et transmission des données.

Les pilotes spécifiques doivent rester indépendants de la logique applicative. Les fonctions d'acquisition brute doivent être séparées des fonctions de calibration, de filtrage et de conversion en unités physiques.

La compatibilité entre différents capteurs doit également tenir compte de leurs interfaces électriques, de leur alimentation, de leurs tensions de sortie et de leur brochage.

---

### 🔌 Interfaces matérielles

| Composant | Interface | Fonction |
|---|---|---|
| BMP280 | I²C | Température et pression |
| SEN0159 / MG-811 | ADC | Acquisition analogique du CO₂ |
| MQ-135 | ADC / GPIO | Qualité de l'air |
| Écran LCD | Selon le modèle retenu | Affichage local |
| Console série | UART | Transmission des données |

Les broches, les canaux ADC et les paramètres de communication doivent être documentés dans les pilotes correspondants.

Toute entrée analogique doit respecter les limites électriques du microcontrôleur.

---

### ⏱️ Gestion temporelle

- Période nominale d'acquisition : 5 secondes.
- Gestion des délais d'initialisation et de stabilisation propres aux capteurs.
- Horodatage des mesures à l'aide des mécanismes temporels du microcontrôleur.
- Séparation entre l'acquisition des données et leur affichage afin de limiter les blocages de la boucle principale.

---

### 💻 Environnement de développement

Cette section documente l'environnement utilisé pour développer, compiler et maintenir le projet. Elle vise à permettre à chaque membre de l'équipe de reproduire les opérations sur son propre poste de travail.

#### 1. Environnement recommandé

| Outil | Utilité | Configuration |
|---|---|---|
| Windows 11 | Système de développement utilisé | Recommandé pour reproduire l'environnement actuel |
| PowerShell 7 | Terminal et automatisation des commandes | Version 7 ou supérieure |
| MCUXpresso IDE | Environnement de développement NXP | Version 25.6 utilisée pour le développement |
| MCUXpresso SDK | Bibliothèques et pilotes NXP | SDK correspondant au MKL27Z |
| ARM GNU Toolchain | Compilation et liaison ARM | Fournie avec MCUXpresso IDE |
| GNU Make | Exécution des Makefiles générés | Fournie avec MCUXpresso IDE |
| Git | Gestion des versions | Requis pour récupérer et contribuer au dépôt |

Les versions indiquées correspondent à l'environnement de développement utilisé pendant le projet. Les membres de l'équipe peuvent utiliser une installation différente, à condition de conserver la compatibilité avec le microcontrôleur, le SDK et les paramètres du projet.

#### 2. Utilisation de PowerShell

Les commandes documentées dans ce README utilisent **PowerShell 7**.

PowerShell sert à :
- naviguer dans le dépôt ;
- vérifier la présence des outils ;
- compiler le projet ;
- inspecter les fichiers et les résultats ;
- gérer les branches et les commits Git.

PowerShell n'est pas une dépendance du programme embarqué. Le code C/C++ ne dépend pas de Windows ni de PowerShell.

**Ouvrir le dépôt :**

```powershell
# Se placer à la racine du dépôt
Set-Location "C:\Users\<UTILISATEUR>\Desktop\INF1633"

# Confirmer le répertoire courant
Get-Location

# Afficher les fichiers et dossiers du dépôt
Get-ChildItem
```

Remplacer `<UTILISATEUR>` par le nom du dossier utilisateur de la machine concernée.

**Vérifier les outils :**

```powershell
# Vérifier la version de PowerShell
$PSVersionTable.PSVersion

# Vérifier Git
git --version

# Vérifier ARM GCC si la commande est accessible dans le PATH
arm-none-eabi-gcc --version

# Vérifier GNU Make si la commande est accessible dans le PATH
make --version
```

Il est possible que `arm-none-eabi-gcc` et `make` ne soient pas reconnus directement dans le terminal. Dans l'installation utilisée pour ce projet, les exécutables sont fournis avec MCUXpresso IDE et ne sont pas nécessairement ajoutés au `PATH` de Windows.

Cela ne signifie pas que les outils sont absents.

#### 3. Détection des outils MCUXpresso

Exemple de configuration utilisée sur le poste de développement initial :

```powershell
# Chemin d'installation de MCUXpresso IDE
$MCUXpresso = "C:\nxp\MCUXpressoIDE_25.6.136"

# Rechercher l'exécutable principal de l'IDE
$Ide = Join-Path $MCUXpresso "ide\mcuxpressoidec.exe"

# Vérifier que l'exécutable existe
Test-Path $Ide
```

Pour retrouver l'installation sur un autre poste, vérifier les dossiers d'installation NXP ou utiliser une recherche Windows.

Les chemins doivent être adaptés à l'installation locale de chaque membre de l'équipe.

**Localiser les outils de compilation :**

```powershell
# Rechercher les exécutables ARM GCC dans l'installation MCUXpresso
Get-ChildItem `
    -Path $MCUXpresso `
    -Filter "arm-none-eabi-gcc.exe" `
    -Recurse `
    -ErrorAction SilentlyContinue |
    Select-Object -ExpandProperty FullName

# Rechercher GNU Make
Get-ChildItem `
    -Path $MCUXpresso `
    -Filter "make.exe" `
    -Recurse `
    -ErrorAction SilentlyContinue |
    Select-Object -ExpandProperty FullName
```

Il est recommandé de conserver les chemins trouvés dans des variables PowerShell plutôt que de les recopier dans toutes les commandes.

#### 4. Structure du projet

Le dépôt contient notamment le projet MCUXpresso dans le sous-dossier `INF1633`.

Structure simplifiée :

```text
INF1633/
│
├── README.md
├── .gitignore
│
└── INF1633/
    ├── .cproject
    ├── .project
    │
    ├── source/
    │   ├── main.c
    │   ├── sensor_mg811.c
    │   ├── sensor_mg811.h
    │   └── ...
    │
    ├── drivers/
    │   ├── fsl_adc16.c
    │   ├── fsl_adc16.h
    │   └── ...
    │
    ├── board/
    │   ├── pin_mux.c
    │   └── ...
    │
    ├── device/
    ├── startup/
    ├── utilities/
    │
    ├── Debug/
    └── Release/
```

Cette représentation est indicative : les dossiers et fichiers évoluent avec l'implémentation du projet.

**Important :**
- Les fichiers `.project` et `.cproject` contiennent des métadonnées importantes pour MCUXpresso.
- Les fichiers du SDK NXP ne doivent pas être remplacés arbitrairement par des versions provenant d'autres projets.
- Les Makefiles des configurations `Debug` et `Release` sont générés par l'environnement de développement.
- Il faut privilégier les modifications des paramètres du projet MCUXpresso plutôt que l'édition manuelle des Makefiles générés.
- Les répertoires de compilation contiennent des fichiers intermédiaires et des exécutables générés.

#### 5. Compilation avec GNU Make

Lorsque les Makefiles sont disponibles, il est possible de compiler directement depuis PowerShell.

Exemple de compilation de la configuration `Debug` :

```powershell
# Définir le chemin de GNU Make
$Make = Join-Path $MCUXpresso `
    "ide\plugins\com.nxp.mcuxpresso.tools.win32_25.6.0.202501151204\buildtools\bin\make.exe"

# Vérifier que GNU Make existe
Test-Path $Make

# Ajouter temporairement les outils nécessaires au PATH
$env:PATH = "$(Split-Path $Make);C:\Program Files\Git\usr\bin;$env:PATH"

# Compiler la configuration Debug
& $Make -C ".\INF1633\Debug" all
```

Pour compiler la configuration `Release` :

```powershell
# Compiler la configuration Release
& $Make -C ".\INF1633\Release" all
```

Les chemins internes de MCUXpresso peuvent varier selon la version installée. Si le chemin de GNU Make diffère, il faut utiliser celui trouvé sur la machine.

Une compilation réussie doit notamment produire un fichier `.axf` et afficher un résultat sans erreur.

#### 6. Compilation avec MCUXpresso en mode commande

Il est également possible d'utiliser le mécanisme de compilation sans interface graphique de MCUXpresso IDE.

Cette méthode est utile lorsqu'on souhaite compiler le projet depuis PowerShell ou automatiser une validation.

**Exemple de configuration :**

```powershell
# Définir le chemin de MCUXpresso IDE
$MCUXpresso = "C:\nxp\MCUXpressoIDE_25.6.136"

# Définir le chemin de l'exécutable en mode commande
$Ide = Join-Path $MCUXpresso "ide\mcuxpressoidec.exe"

# Définir un espace de travail temporaire distinct
$WorkspaceTemp = Join-Path $env:TEMP "MCUXpresso-INF1633-build"

# Créer le dossier s'il n'existe pas
New-Item -ItemType Directory -Path $WorkspaceTemp -Force | Out-Null

# Importer et reconstruire le projet
& $Ide `
    -nosplash `
    -application org.eclipse.cdt.managedbuilder.core.headlessbuild `
    -data $WorkspaceTemp `
    -import (Resolve-Path ".\INF1633").Path `
    -cleanBuild INF1633
```

**Pourquoi utiliser un espace de travail temporaire ?**

MCUXpresso repose sur Eclipse et utilise un espace de travail qui peut être verrouillé lorsqu'il est déjà ouvert dans une autre instance.

Un message comme `Workspace already in use!` indique généralement qu'un autre processus utilise déjà cet espace de travail.

L'utilisation d'un dossier temporaire distinct évite de devoir fermer l'environnement de développement habituel.

Le résultat de compilation doit être vérifié dans la sortie de l'outil. Il faut notamment rechercher :
- `Build Finished`;
- `0 errors`;
- `0 warnings`;
- la génération du fichier `.axf`.

Le code de retour du processus peut parfois ne pas refléter fidèlement le résultat affiché dans le journal de compilation. En cas de divergence, il faut examiner le journal complet et confirmer la présence du fichier de sortie avant de conclure.

**Attention :** la commande de reconstruction utilise la configuration sélectionnée par le projet importé dans l'espace de travail. Il faut donc vérifier le nom du fichier de liaison généré (`INF1633_Debug.ld` ou `INF1633_Release.ld`) pour savoir quelle configuration a réellement été compilée.

#### 7. Résultat de compilation validé

Une compilation complète de la configuration `Release` a été effectuée avec les pilotes MG-811 et ADC16 intégrés.

Résultat observé :

| Indicateur | Résultat |
|---|---:|
| Configuration | Release |
| Fichier généré | `INF1633.axf` |
| Erreurs | 0 |
| Avertissements | 0 |
| Flash utilisée | 3 088 octets |
| Flash disponible | 64 Ko |
| Utilisation de la Flash | 4,71 % |
| SRAM utilisée | 2 236 octets |
| SRAM disponible | 16 Ko |
| Utilisation de la SRAM | 13,65 % |

La commande de liaison contenait notamment :

```text
./source/sensor_mg811.o
./drivers/fsl_adc16.o
```

Cela confirme que le pilote MG-811 et le pilote ADC16 ont été compilés et intégrés à l'édition de liens de cette compilation.

**Limite de cette validation :** une compilation réussie confirme la cohérence de la compilation et de l'édition de liens. Elle ne confirme pas encore le fonctionnement électrique du capteur, la précision des mesures ni le fonctionnement sur la carte physique.

---

### 🧪 Pilote MG-811 — Acquisition ADC

#### 1. Rôle du pilote

Le pilote `sensor_mg811.c/.h` fournit une interface d'acquisition analogique brute pour le MG-811.

Il sépare l'accès matériel ADC de la logique applicative et ne réalise pas directement la conversion de la valeur brute en concentration de CO₂.

#### 2. Configuration matérielle actuelle

| Paramètre | Valeur |
|---|---|
| Convertisseur | ADC0 |
| Canal | ADC0_SE1 |
| Broche | PTE16 |
| Connecteur | J4[2] / A0 |
| Résolution | 12 bits |
| Mode | Conversion simple |
| Acquisition | Polling |
| Groupe de conversion | 0 |

La correspondance entre PTE16 et ADC0_SE1 est définie dans la configuration de broches du projet (`board/pin_mux.c`).

#### 3. Interface publique

Le fichier `sensor_mg811.h` expose actuellement deux fonctions :

```c
bool MG811_Init(void);

bool Read_Sensor_Raw(uint16_t *rawValue);
```

- `MG811_Init()` configure les ressources matérielles nécessaires à l'utilisation de l'ADC.
- `Read_Sensor_Raw()` déclenche une conversion et récupère la valeur brute.
- Le paramètre `rawValue` reçoit la valeur convertie sur 12 bits.
- La fonction retourne `true` lorsque la conversion est terminée avec succès.
- Si le pointeur est invalide, si le pilote n'est pas initialisé ou si le délai d'attente est dépassé, la fonction retourne `false`.
- Lorsque le pointeur est valide, sa valeur est initialisée à zéro avant la tentative de conversion.

#### 4. Gestion de la fin de conversion

Le pilote utilise le drapeau :

```c
kADC16_ChannelConversionDoneFlag
```

La fonction attend que le matériel signale la fin de conversion.

La boucle de polling est limitée par un compteur d'itérations afin d'éviter une attente infinie.

La lecture du résultat est effectuée avec :

```c
ADC16_GetChannelConversionValue(ADC0, 0U);
```

Sur ce périphérique, la lecture du registre de résultat permet également d'effacer le drapeau de fin de conversion.

#### 5. Dépendances NXP

Le pilote utilise notamment :

```c
#include "fsl_adc16.h"
#include "fsl_clock.h"
#include "fsl_port.h"
```

Les fonctions ADC sont fournies par le SDK NXP.

Le fichier `fsl_adc16.c` doit donc être présent dans le projet et intégré à la compilation.

#### 6. Calibration et conversion en ppm

La lecture ADC fournit une valeur numérique brute. Elle ne représente pas directement une concentration de CO₂.

La conversion en ppm nécessite notamment :
- une caractérisation du signal analogique ;
- une calibration appropriée ;
- la prise en compte des caractéristiques de la sonde ;
- une stabilisation suffisante du capteur ;
- une validation des résultats avec une référence fiable.

La calibration automatique du convertisseur ADC n'est pas encore effectuée par le pilote actuel. La fonction `ADC16_Init()` du SDK retourne `void`, ce qui signifie que `MG811_Init()` confirme l'exécution de la séquence de configuration logicielle, et non une validation complète du fonctionnement matériel.

---

### 🔒 Sécurité et branchement des capteurs

Avant tout branchement sur la FRDM-KL27Z :

1. Vérifier la tension d'alimentation requise par le module.
2. Vérifier la tension maximale de sa sortie.
3. Vérifier les limites absolues et les caractéristiques électriques de l'entrée du microcontrôleur dans la documentation NXP.
4. Vérifier le brochage du connecteur et la référence de masse.
5. Calculer et valider tout circuit d'adaptation de tension nécessaire.
6. Vérifier la capacité de l'alimentation disponible.

**Le SEN0159 ne doit pas être branché directement sur l'entrée ADC du KL27Z.** Sa sortie analogique peut atteindre une tension supérieure à celle admissible par le microcontrôleur. Le pont diviseur est obligatoire pour cette interface.

Le fait qu'un capteur utilise une sortie analogique ne signifie pas que sa sortie est électriquement compatible avec l'entrée ADC.

---

### 📌 État d'avancement

Les pilotes doivent être intégrés et validés progressivement.

La configuration matérielle par défaut est le module SEN0159 pour le CO₂, le BMP280 pour la température et la pression, et le MQ-135 comme capteur secondaire optionnel.

Le pilote MG-811 est conservé comme pilote de référence, même si le module SEN0159 est celui utilisé par défaut dans le projet.

**Validation technique réalisée :**
- Le pilote MG-811 est présent dans le projet.
- La configuration ADC0_SE1 / PTE16 est identifiée.
- Le polling de fin de conversion est implémenté avec un délai d'attente borné.
- Le pilote ADC16 du SDK est intégré à l'édition de liens.
- Une compilation complète Release a été réalisée avec zéro erreur et zéro avertissement.

**Validations restant à effectuer :**
- Essais du pilote sur la carte FRDM-KL27Z physique.
- Vérification électrique du circuit d'adaptation de tension.
- Validation de l'acquisition avec le capteur réellement branché.
- Calibration et conversion de la valeur brute en ppm.
- Validation fonctionnelle de l'ensemble des périphériques et de l'application.

---

### 🤝 Contribution et gestion des versions

Le dépôt Git constitue la source de référence du projet.

Avant de commencer une modification :

```powershell
# Vérifier le répertoire courant
Get-Location

# Vérifier la branche active
git branch --show-current

# Vérifier l'état du dépôt
git status
```

Pour récupérer les dernières modifications :

```powershell
# Récupérer les références du dépôt distant
git fetch origin

# Mettre à jour la branche principale locale
git pull
```

Pour travailler sur une fonctionnalité, utiliser une branche dédiée :

```powershell
# Créer une branche de fonctionnalité
git switch -c feature/nom-de-la-fonctionnalite
```

Avant de créer un commit :

```powershell
# Examiner les fichiers modifiés
git status --short

# Examiner les modifications
git diff

# Vérifier les erreurs de whitespace
git diff --check
```

Ajouter uniquement les fichiers pertinents :

```powershell
# Exemple : ajouter explicitement les fichiers concernés
git add README.md
git add INF1633/source/sensor_mg811.c
git add INF1633/source/sensor_mg811.h

# Vérifier le contenu préparé pour le commit
git diff --cached
```

Créer ensuite un commit descriptif :

```powershell
git commit -m "drivers: ajouter la lecture brute du MG-811"
```

Publier la branche :

```powershell
git push -u origin HEAD
```

**Bonnes pratiques :**
- Éviter `git add .` sans avoir préalablement examiné tous les fichiers.
- Ne pas versionner les fichiers temporaires, les sauvegardes locales ou les exécutables de compilation, sauf si cela est explicitement nécessaire.
- Conserver les fichiers de configuration MCUXpresso requis pour reproduire le projet.
- Vérifier le contenu du commit avant de pousser les changements.
- Utiliser des messages de commit qui décrivent clairement la fonctionnalité ou la correction apportée.

---

### 📚 Documentation technique

Pour les détails matériels et logiciels, consulter les documentations officielles correspondant aux composants réellement utilisés :

- Documentation NXP FRDM-KL27Z et MKL27Z64VLH4.
- Documentation du SDK MCUXpresso.
- Documentation du pilote ADC16 du SDK NXP.
- Documentation du module DFRobot Gravity SEN0159.
- Documentation de la sonde MG-811.
- Documentation du capteur BMP280.

Les limites électriques, les caractéristiques de conversion et les procédures de calibration doivent toujours être validées à partir des fiches techniques correspondant aux références matérielles exactes.

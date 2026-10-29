# INF1633 — Programmation de systèmes embarqués en C/C++
## Automne 2026
## PROJET DE FIN DE SESSION — [50%]
### LECTURE DES DONNÉES DE CAPTEURS SUR ÉCRAN LCD VIA LA CARTE KL27Z

**Délai de remise :** Lundi 08 décembre 2026 à 8h00  
**Présentations :** Jeudi 10 décembre 2026 (20 min présentation + 10 min questions)

---

### 👥 Membres de l'équipe
*   **Antoine Mailhot**
*   **William Lamoureux**
*   **Micaël Racine-Leblanc**

---

### 📝 Titre du Projet
**Groupe 1 : Idée 1 et idée 7 : Projet Unifié : Station de surveillance environnementale (Température, Pression et CO₂)**

---

### 🎯 Objectif général
Développer un programme en langage C/C++ sur la carte FRDM-KL27Z (microcontrôleur MKL27Z64VLH4) exploitant le bus I²C pour communiquer simultanément avec un capteur de température/pression BMP280 et un capteur de CO₂ SEN0159 / MG-811.
Le système doit assurer une lecture en temps réel, un affichage double (console série et écran LCD) et une gestion d'alertes visuelles en cas de dépassement des seuils critiques.

---

### 🔍 Alignement avec les exigences générales du cours
*   **Utilisation de la carte KL27Z :** Microcontrôleur central MKL27Z64VLH4 utilisé pour orchestrer le système.
*   **Connexion d’au moins deux capteurs via I2C :** Intégration simultanée du capteur BMP280 et du capteur MG-811.
*   **Lecture des résultats via l’écran LCD :** Affichage local des mesures pour une consultation directe.
*   **Jeu de LED pour communiquer :** Utilisation des DEL verte et rouge pour indiquer l'état du système et les alarmes.
*   **Utilisation de l’horloge interne de la carte :** Configuration des compteurs internes pour dater l’affichage des données collectées.

---

### ⚙️ Fonctionnalités principales

#### 1. Acquisition de données (toutes les 5 secondes)
*   **BMP280 :** Lecture de la température (°C) et de la pression atmosphérique (hPa) via le bus I²C.
*   **MG-811 :** Lecture de la concentration de CO₂ (ppm) via I²C (ou via ADC selon le module d'interface utilisé).

#### 2. Affichage des mesures
*   **Local :** Affichage en temps réel des données environnementales sur l'écran LCD.
*   **À distance :** Envoi et mise en forme des données sur la console série (UART).

#### 3. Indicateurs visuels & Alertes
*   **Fonctionnement normal :** Clignotement de la LED verte à chaque cycle de lecture (toutes les 5 secondes).
*   **Alerte Température :** Allumage d'une LED rouge si la température dépasse 30 °C.
*   **Alerte CO₂ :** Allumage d'une LED rouge (ou gestion d'une seconde LED d'alerte) si la concentration en CO₂ dépasse 1000 ppm.

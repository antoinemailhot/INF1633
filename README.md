# INF1633

# Auteur :
## - Antoine Mailhot
## - Lamoureux William
## - Racine-Leblanc Micaël

# Projet : Afficher la teneur en Co2 dans l’air à l’aide du capteur SEN0159 / MG-811 et la carte FRDM-KL27Z

# Objectif du projet :
## Développer un programme en langage C/C++ sur la carte FRDM-KL27Z utilisant le bus I²C pour communiquer avec un capteur MG-811. Le programme doit lire et afficher la concentration en Co2 dans l’air sur la console série après chaque 5 secondes en clignotant la LED verte, et allumer une LED rouge lorsque la concentration en Co2 dépasse la limite prédéfinie de 1000 ppm.
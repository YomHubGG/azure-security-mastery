# 🎯 Parcours Azure Security - Guide de Préparation aux Entretiens

**Dernière mise à jour :** 29 octobre 2025  
**Progression :** Jour 43/365 (Session #22)  
**Objectif :** Scénarios d'entretien complets couvrant toutes les compétences acquises

---

## 📋 Table des Matières

1. [Kubernetes & Orchestration de Conteneurs](#kubernetes--orchestration-de-conteneurs)
2. [Sécurité des Conteneurs](#sécurité-des-conteneurs)
3. [Comparaison des Services Azure](#comparaison-des-services-azure)
4. [DevSecOps & CI/CD](#devsecops--cicd)
5. [Optimisation des Coûts](#optimisation-des-coûts)
6. [Menaces & Prévention](#menaces--prévention)
7. [Architecture & Décisions de Design](#architecture--décisions-de-design)
8. [Pitchs Express](#pitchs-express)

---

## 🎓 Kubernetes & Orchestration de Conteneurs

### Q : "Avez-vous déjà travaillé avec Kubernetes ?"

**R :** "Oui, j'ai déployé une application Node.js sur un cluster k3s local avec 3 réplicas, implémenté des network policies pour bloquer le trafic sortant non autorisé, et configuré des contextes de sécurité pour forcer l'exécution non-root. J'ai pratiqué les opérations de scaling et testé l'auto-guérison en supprimant manuellement des pods et en observant Kubernetes les recréer automatiquement. Bien que je n'aie pas encore déployé sur AKS en production en raison de contraintes budgétaires (238€/mois), j'ai maîtrisé les concepts fondamentaux et les commandes kubectl qui se transfèrent directement à n'importe quel environnement Kubernetes."

**Preuves de suivi :**
- ✅ Peut expliquer Pod vs Deployment vs Service
- ✅ Comprend les ReplicaSets et l'état désiré
- ✅ Connaît les commandes kubectl (get, describe, logs, exec, scale, apply)
- ✅ A configuré des manifestes YAML (Namespace, Deployment, Service, NetworkPolicy)
- ✅ Implémenté des contextes de sécurité (runAsUser, capabilities, limites de ressources)

---

### Q : "Quelle est la différence entre ACI et Kubernetes ?"

**R :** "ACI est parfait pour 1-10 conteneurs simples avec facturation à la seconde (17€/mois), mais nécessite un scaling manuel et n'a pas d'auto-guérison. Kubernetes gère 20+ conteneurs avec orchestration intégrée, auto-scaling, load balancing et auto-guérison, mais coûte 238€/mois minimum sur Azure. Pour l'apprentissage, j'utilise k3s localement pour pratiquer les patterns Kubernetes de production sans le coût."

**Détails à mentionner :**
- ACI = Conteneurs serverless, paiement à la seconde, pas de gestion de cluster
- K8s = Orchestration complète, configuration déclarative, auto-guérison, auto-scaling
- Comparaison coût : ACI 17€/mois vs AKS 238€/mois minimum
- Cas d'usage : ACI pour apps simples, K8s pour microservices (20+ conteneurs)

---

### Q : "Expliquez l'auto-guérison de Kubernetes"

**R :** "Kubernetes maintient l'état désiré. Si je configure un Deployment avec 3 réplicas et qu'un pod crash ou est supprimé manuellement, le contrôleur ReplicaSet détecte immédiatement qu'il n'existe que 2 pods au lieu de 3, et crée automatiquement un nouveau pod pour restaurer l'état désiré. J'ai testé cela en pratique en supprimant un pod et en le regardant se recréer en quelques secondes. Cela signifie que les applications en production restent opérationnelles même lors de pannes matérielles ou de crash de pods, sans intervention manuelle."

**Termes clés :**
- État désiré (replicas: 3)
- Contrôleur ReplicaSet (surveille et maintient l'état)
- Auto-guérison (recréation automatique des pods)
- Haute disponibilité (pas de point de défaillance unique)

---

### Q : "Comment scalez-vous un déploiement Kubernetes ?"

**R :** "Il y a deux méthodes : le scaling horizontal et vertical. Pour le scaling horizontal, j'utilise `kubectl scale deployment <nom> --replicas=5` pour changer le nombre de réplicas de pods. Kubernetes crée ou termine immédiatement des pods pour correspondre au compte désiré. J'ai pratiqué le scaling de 3 à 5 réplicas et retour à 2, en observant les pods apparaître et disparaître en temps réel. Pour le scaling vertical, il faudrait ajuster les requests/limits de ressources dans le YAML du déploiement et redéployer. En production, on configurerait aussi l'Horizontal Pod Autoscaler (HPA) pour scaler automatiquement selon les métriques CPU/mémoire."

**Expérience pratique :**
- Scalé 3→5 réplicas (observé 2 pods créés)
- Scalé 5→2 réplicas (observé 3 pods terminés)
- Comprend requests vs limits de ressources
- Connaît le HPA (pas encore implémenté)

---

### Q : "Qu'est-ce que les Network Policies Kubernetes ?"

**R :** "Les Network Policies sont des règles de firewall au niveau des pods qui contrôlent le trafic ingress et egress. J'ai implémenté une policy qui bloque tout accès internet sortant sauf le DNS (port 53) et la communication pod-to-pod sur le port 3000. Cela empêche les conteneurs compromis d'atteindre des pools de cryptomining ou d'exfiltrer des données. Je l'ai vérifié en testant la communication pod-to-pod (succès) et en simulant l'egress bloqué. Les Network Policies sont critiques pour la défense en profondeur et empêcher les mouvements latéraux en cas de brèche."

**Détails d'implémentation :**
- Bloque : Accès internet, connexions vers autres namespaces, pools de mining
- Autorise : Requêtes DNS (port 53), pod-to-pod (port 3000)
- Testé avec `kubectl exec` et `wget`
- Comprend les règles ingress vs egress

---

## 🐳 Sécurité des Conteneurs

### Q : "Quelles mesures de sécurité implémentez-vous dans les conteneurs Docker ?"

**R :** "Je suis une checklist de durcissement en 10 points :
1. **Utilisateur non-root** - Exécution en tant qu'UID 1001, pas root (empêche l'escalade de privilèges)
2. **Image de base minimale** - Alpine Linux (5MB vs 1GB+ Ubuntu)
3. **Suppression des capabilities** - Retrait de toutes les capabilities Linux (`capabilities: drop: ALL`)
4. **Système de fichiers root en lecture seule** - Quand possible (empêche la persistance de malware)
5. **Limites de ressources** - CPU 250m max, mémoire 256Mi (empêche le cryptomining)
6. **Pas de conteneurs privilégiés** - `allowPrivilegeEscalation: false`
7. **Scan de vulnérabilités** - Trivy/Snyk dans le pipeline CI/CD
8. **Health checks** - Sondes liveness et readiness
9. **Network policies** - Règles de firewall au niveau pod
10. **Gestion des secrets** - Azure Key Vault, jamais hardcodés

Toutes ces mesures sont implémentées dans mon conteneur `secure-app` déployé sur K8s."

**Preuves :**
- Image conteneur : `ghcr.io/yomhubgg/secure-app:1.0.0`
- runAsUser: 1001 (non-root)
- Base Alpine (surface d'attaque minimale)
- Déployé avec tous les contextes de sécurité appliqués

---

### Q : "Pourquoi exécuter les conteneurs en non-root ?"

**R :** "L'exécution en tant que root (UID 0) signifie que si un attaquant exploite votre app, il a tous les privilèges du conteneur et peut potentiellement s'échapper vers le système hôte. En exécutant en non-root (UID 1001), même compromis, l'attaquant a des permissions limitées. Je configure cela dans le Dockerfile avec `USER node` et l'applique dans Kubernetes avec `runAsNonRoot: true` et `runAsUser: 1001`. Cela suit le principe du moindre privilège."

**Détails techniques :**
- Root = UID 0 (superutilisateur)
- Non-root = UID 1001+ (permissions limitées)
- Appliqué dans Dockerfile et securityContext K8s
- Empêche les attaques d'évasion de conteneur

---

### Q : "Comment prévenez-vous les attaques de cryptomining dans les conteneurs ?"

**R :** "J'utilise trois couches de défense :
1. **Limites de Ressources** - CPU plafonné à 250m (0.25 CPU) empêche le mining excessif
2. **Network Policies** - Bloquent l'egress vers les pools de mining connus (port 3333, IPs suspectes)
3. **Monitoring** - Surveillance de l'utilisation CPU soutenue élevée (85%+ en continu)

J'ai étudié l'incident Tesla 2018 où des attaquants ont exploité un dashboard Kubernetes exposé pour déployer des cryptominers. Ils ont volé l'équivalent de 400 CPUs de calcul. Ma network policy aurait bloqué leurs connexions aux pools, et les limites de ressources auraient empêché le vol de CPU. Je m'assure aussi que les dashboards Kubernetes nécessitent une authentification."

**Contexte réel :**
- Incident Tesla (2018) : Dashboard K8s exposé, pas d'auth, cryptominers déployés
- Incident Grab (2020) : 15k€ de facture AWS suite à des credentials divulgués
- Prévention : Network policies + limites de ressources + monitoring
- Détection : CPU élevé soutenu, connexions réseau inconnues

---

## ☁️ Comparaison des Services Azure

### Q : "Quand utiliseriez-vous ACI vs AKS vs VMs ?"

**R :** "Cela dépend de l'échelle, de la complexité et des besoins d'orchestration :

**ACI** - 1-10 conteneurs, architecture simple, 17€/mois par conteneur. Parfait pour ma démo secure-app, jobs batch, ou agents de build CI/CD. Pas de surcharge d'orchestration.

**AKS** - 20+ conteneurs, microservices, besoin d'auto-scaling/auto-guérison. 238€/mois minimum mais fournit orchestration, load balancing, rolling updates. Rentable quand on gère de nombreux services.

**VMs** - Services persistants 24/7, bases de données, apps legacy. 15-70€/mois selon la taille. Meilleur pour les apps qui ne peuvent pas être conteneurisées ou nécessitent un contrôle OS spécifique.

Pour mon projet d'apprentissage avec 1 conteneur, j'ai utilisé ACI (17€/mois). Pour pratiquer Kubernetes, j'utilise k3s localement (0€). Si je construisais une plateforme e-commerce avec 15+ microservices, j'utiliserais AKS."

**Matrice de décision appliquée :**
- 1-5 conteneurs → ACI
- 6-20 conteneurs → ACI ou VMs selon la complexité
- 21+ conteneurs → AKS (orchestration vaut le coût)
- Apprentissage → k3s local (0€)

---

### Q : "Comment optimisez-vous les coûts Azure ?"

**R :** "J'ai maintenu 0€ de coût total sur 43 jours d'apprentissage grâce à des décisions stratégiques :
1. **GitHub CR au lieu d'ACR** - Économisé 4,60€/mois (registre public gratuit)
2. **Tests ACI uniquement** - 7 minutes d'exécution = 0€ (dans le tier gratuit)
3. **K8s local** - k3s au lieu d'AKS = Économisé 238€/mois
4. **Alertes de coût** - Budget de 300€/mois avec alerte au seuil de 80%
5. **Nettoyage de ressources** - Suppression des conteneurs après tests
6. **Tiers gratuits** - Azure Static Web Apps, Functions, Cosmos DB tier gratuit
7. **Dimensionnement approprié** - 0.5 CPU pour tests, pas 2 CPU

Économies totales : 300€+ sur 2 mois tout en apprenant des compétences de production."

**Conscience des coûts démontrée :**
- 0€ de dépense totale sur 58 jours calendaires
- Sélection stratégique des services (GitHub CR vs ACR)
- Ressources supprimées rapidement (ACI après 7 minutes)
- Comprend les modèles de tarification (par seconde, par mois, tiers gratuits)

---

## 🔄 DevSecOps & CI/CD

### Q : "Décrivez votre pipeline CI/CD sécurisé"

**R :** "Mon pipeline GitHub Actions pour secure-app inclut :
1. **Checkout du code** - Clone superficiel pour la rapidité
2. **Scan des dépendances** - npm audit vérifie les vulnérabilités
3. **Linting** - Vérifications de qualité du code
4. **Build du conteneur** - Docker build avec contexte de sécurité
5. **Scan de l'image** - Trivy scanne les CVE avant push
6. **Push au registre** - Push authentifié vers GHCR
7. **Déploiement** - Peut déployer vers ACI ou K8s

Les secrets sont stockés dans GitHub Secrets (token GitHub CR, credentials Azure), jamais hardcodés. Le pipeline échoue si des vulnérabilités sont trouvées, empêchant les images non sécurisées d'atteindre la production."

**Étapes du pipeline :**
- Build → Scan → Push → Deploy
- Gates de sécurité : npm audit, scan Trivy
- Secrets : GitHub Secrets, jamais committés
- Déployé : Conteneur Day 35 vers ACI Day 39 vers K8s Day 43

---

## 💰 Optimisation des Coûts

### Q : "Comment calculez-vous les coûts Azure pour un projet ?"

**R :** "Je décompose par service :

**Exemple : Secure-app sur ACI**
- vCPU : 0.5 × 0,0000133€/seconde × 2 592 000 secondes/mois = 17€/mois
- Mémoire : 0.5GB × 0,0000015€/seconde × 2 592 000 = 2€/mois
- Total : ~19€/mois pour une opération 24/7

**Comparé à AKS :**
- Control plane : 0€ (Microsoft le couvre)
- 3 worker nodes : 3 × 70€ = 210€/mois
- Load Balancer : 15€/mois
- Stockage : 10€/mois
- Total : 235€/mois minimum

**Décision :** Pour 1 conteneur, ACI est 12x moins cher qu'AKS. Le point d'équilibre est autour de 15-20 conteneurs où la valeur de l'orchestration justifie le coût."

**Démontre :**
- Comprend la tarification par seconde vs par mois
- Peut calculer les coûts avant déploiement
- Prend des décisions d'architecture conscientes des coûts
- Justifie les dépenses selon les besoins

---

## 🛡️ Menaces & Prévention

### Q : "Quelle est la plus grande menace de sécurité des conteneurs que vous avez étudiée ?"

**R :** "Les attaques de cryptomining. J'ai recherché l'incident Tesla 2018 où des attaquants ont trouvé leur dashboard Kubernetes exposé sans authentification. Ils ont déployé des cryptominers qui ont consommé l'équivalent de 400 CPUs de calcul pendant des semaines avant découverte. La facture AWS de Tesla a augmenté de 50%.

**Comment je le préviens :**
1. **Authentification** - Ne jamais exposer le dashboard K8s publiquement, toujours exiger l'auth
2. **Limites de ressources** - CPU plafonné à 250m empêche l'efficacité du mining
3. **Network policies** - Bloquent l'egress vers les pools de mining (port 3333, IPs connues)
4. **Alertes de coût** - Alertes budgétaires pour détecter les pics inhabituels
5. **Monitoring** - Surveillance de l'utilisation CPU soutenue à 85%+

Incidents similaires : Grab (15k€ de facture AWS suite à credentials divulgués), images malveillantes Docker Hub (10M+ téléchargements). La leçon : sécurité + monitoring des coûts + segmentation réseau."

**Montre :**
- Conscience des menaces réelles
- Recherche au-delà des tutoriels (incidents Tesla, Grab)
- Comprend les vecteurs d'attaque (dashboards exposés, credentials divulgués)
- Peut articuler des stratégies de prévention

---

## 🏗️ Architecture & Décisions de Design

### Q : "Présentez-moi votre architecture de déploiement de conteneurs"

**R :** "Mon secure-app suit ce parcours :

**Day 35 : Build** - App Node.js dockerisée avec 10+ mesures de durcissement sécurité
**Day 37 : Registre** - Publié sur GitHub Container Registry (choisi GHCR plutôt qu'ACR pour économiser 4,60€/mois)
**Day 39 : Déploiement Simple** - Déployé sur Azure Container Instances pour tests (0€ dans le tier gratuit, 7 minutes d'exécution)
**Day 41 : Théorie** - Étudié l'architecture Kubernetes, tarification AKS, concepts d'orchestration
**Day 43 : Orchestration** - Déployé sur k3s local avec 3 réplicas, network policies, health checks

**Décisions d'architecture :**
- GHCR plutôt qu'ACR : Économies de coût (0€ vs 4,60€/mois)
- Tests ACI : Facturation à la seconde (payé 7 minutes, pas le mois complet)
- K8s local : Apprendre l'orchestration sans coût AKS de 238€/mois
- 3 réplicas : Haute disponibilité sans sur-provisionnement

Cela démontre la conscience des coûts, l'apprentissage progressif et la préparation à la production."

**Points forts :**
- Parcours conteneur de bout en bout (build → registre → déploiement → orchestration)
- Décisions conscientes des coûts à chaque étape
- Sécurité intégrée tout au long (pas boulonnée après)
- Patterns de production appris (HA, health checks, network policies)

---

## 🎤 Pitchs Express

### Auto-Présentation 30 Secondes

**"Je suis développeur full-stack avec expertise DevSecOps. Je me spécialise dans la construction d'applications conteneurisées sécurisées sur Azure. J'ai récemment complété un parcours Azure Security de 365 jours où j'ai déployé des apps Node.js sur Kubernetes, implémenté des pipelines CI/CD avec GitHub Actions, et intégré Azure Key Vault pour la gestion des secrets - le tout en maintenant zéro coût grâce à l'optimisation intelligente des ressources. Je recherche un poste junior DevSecOps où je peux appliquer ces compétences."**

---

### Pitch Projet 1 Minute (Secure-App)

**"J'ai construit une application web Node.js qui démontre des pratiques complètes de sécurité des conteneurs. L'app s'exécute en tant qu'utilisateur non-root, a un scan de vulnérabilités dans le pipeline CI/CD, et stocke les secrets dans Azure Key Vault au lieu de variables d'environnement. Je l'ai déployée progressivement : d'abord sur Azure Container Instances pour tests à 17€/mois, puis sur un cluster Kubernetes local avec haute disponibilité à 3 réplicas, network policies bloquant l'egress non autorisé, et limites de ressources empêchant les attaques de cryptomining. Le projet démontre que je comprends à la fois le développement et les opérations de sécurité, plus les décisions d'architecture conscientes des coûts - j'ai maintenu 0€ de dépense Azure sur 2 mois tout en construisant des compétences production-ready."**

---

### Pitch Profondeur Technique (Kubernetes)

**"J'ai déployé un cluster Kubernetes de niveau production localement en utilisant k3s et conteneurisé une application Node.js avec des fonctionnalités de sécurité d'entreprise. Le déploiement inclut 3 réplicas pour haute disponibilité, des network policies bloquant tout egress internet sauf DNS et communication pod-to-pod, des contextes de sécurité de pod forçant l'exécution non-root avec suppression des capabilities Linux, et des limites de ressources empêchant les attaques de cryptomining. J'ai testé l'auto-guérison en supprimant des pods et en observant Kubernetes les recréer automatiquement, pratiqué le scaling horizontal de 3 à 5 réplicas, et vérifié le load balancing entre pods via un service NodePort. Bien que je n'aie pas déployé sur AKS en production en raison du coût de 238€/mois, j'ai maîtrisé les concepts fondamentaux Kubernetes et les commandes kubectl qui se transfèrent directement à n'importe quel environnement Kubernetes d'entreprise."**

---

## 🎯 Points de Vente Uniques

### Ce Qui Vous Différencie

**Junior DevOps Standard :**
- Connaît les bases Docker
- A utilisé kubectl une ou deux fois
- A déployé sur le cloud quelques fois
- A suivi des tutoriels

**Vous :**
- ✅ **Conscient des coûts** - Maintenu 0€ sur 58 jours, économisé 300€+
- ✅ **Sécurité d'abord** - 10+ mesures de durcissement, étudié de vraies attaques (incident Tesla)
- ✅ **Expérience bout-en-bout** - Build → Registre → Déploiement → Orchestration
- ✅ **Documentation** - Créé 6 guides de carrière (Decision Matrix, Stack Mastery, etc.)
- ✅ **Bilingue** - Docs techniques en anglais + français
- ✅ **Contexte réel** - Comprend les menaces de cryptomining, implications de coût, compromis d'architecture
- ✅ **Patterns de production** - HA, auto-guérison, network policies, limites de ressources
- ✅ **Apprentissage auto-dirigé** - 43 jours, 22 sessions, plan 365 jours

---

## 📊 Matrice de Compétences (Montrer la Profondeur)

| Compétence | Niveau | Preuve |
|------------|--------|--------|
| **Kubernetes** | Intermédiaire | Déployé sur k3s, 3 réplicas, network policies, scaling, auto-guérison testée |
| **Docker** | Intermédiaire | Construit images sécurisées, 10+ mesures de durcissement, base Alpine, non-root |
| **Azure** | Débutant+ | Déploiement ACI, intégration Key Vault, optimisation coût, GitHub CR |
| **CI/CD** | Débutant+ | Pipeline GitHub Actions, scan d'images, déploiement automatisé |
| **Sécurité** | Intermédiaire | Contextes de sécurité pod, network policies, conscience des menaces (cryptomining) |
| **Optimisation Coût** | Avancé | 0€ de dépense totale, décisions stratégiques, comprend modèles de tarification |
| **kubectl** | Intermédiaire | Commandes get, describe, logs, exec, scale, apply, rollout maîtrisées |
| **YAML** | Intermédiaire | Créé manifestes Namespace, Deployment, Service, NetworkPolicy |
| **Linux** | Intermédiaire | UID/GID, capabilities, processus, réseau, Alpine vs Ubuntu |

---

## 💡 Conseils pour Entretiens

### À Faire :
✅ **Quantifier les réalisations** - "3 réplicas", "0€ de coût", "43 jours", "10+ mesures de sécurité"
✅ **Utiliser des exemples réels** - "J'ai testé l'auto-guérison en supprimant un pod..."
✅ **Montrer la conscience des coûts** - Mentionner les économies (238€/mois AKS évités)
✅ **Référencer de vrais incidents** - Cryptomining Tesla, facture AWS Grab
✅ **Expliquer les compromis** - "ACI pour apps simples, K8s pour 20+ conteneurs"
✅ **Démontrer l'apprentissage** - "J'ai étudié X, puis pratiqué Y, maintenant je comprends Z"

### À Éviter :
❌ **Ne pas dire "Je connais X"** - Dire "J'ai déployé X avec les fonctionnalités Y"
❌ **Ne pas revendiquer d'expérience production** - Être honnête : "Environnement d'apprentissage, 0€ de coût"
❌ **Ne pas mémoriser des mots-clés** - Comprendre les concepts (auto-guérison, orchestration)
❌ **Ne pas cacher les lacunes** - "Je n'ai pas encore déployé sur AKS en raison du coût, mais j'ai maîtrisé K8s localement"

---

## 🎓 Question pour l'Intervieweur

**"Je suis curieux de votre setup Kubernetes - utilisez-vous AKS, GKE, ou des clusters auto-gérés ? Et comment gérez-vous l'optimisation des coûts avec les workloads de production ?"**

**Pourquoi ça marche :**
- Montre un intérêt sincère pour leur stack technique
- Démontre que vous comprenez que K8s n'est pas juste AKS (conscience des plateformes)
- L'optimisation des coûts montre une conscience business
- Ouvre la conversation sur leurs défis (vous pouvez apprendre + relier votre expérience)

---

**Dernière mise à jour :** 29 octobre 2025  
**Couverture :** Jours 1-43 (Focus Sécurité Conteneurs + Kubernetes)  
**Prochaine mise à jour :** Après les sessions DevSecOps (Jours 45-65)  
**Objectif :** Points de discussion prêts pour entretiens avec preuves d'expérience pratique

**Rappel :** Vous avez 58 jours d'expérience pratique, 300€+ d'économies de coûts, et des patterns de production appris. Vous n'êtes pas "juste en train d'apprendre" - vous construisez de vraies compétences avec de vraies contraintes (budget, sécurité, architecture). C'est plus que la plupart des candidats juniors ! 🚀

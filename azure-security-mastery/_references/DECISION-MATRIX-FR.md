# 🎯 Matrice de Décision Cloud - Quand Utiliser Quoi ?

**Dernière mise à jour :** 25 octobre 2025  
**Langue :** Français  
**Objectif :** Prendre les bonnes décisions architecturales (AKS vs ACI vs VMs vs Functions)

---

## 📊 Matrice de Décision Rapide

| Ton Besoin | Utilise Ça | Coût/Mois | Pourquoi |
|-----------|------------|-----------|----------|
| **1-5 conteneurs, appli simple** | ACI | 10-30€ | Pas besoin d'orchestration |
| **6-20 conteneurs, en croissance** | ACI + Load Balancer | 50-100€ | Encore gérable manuellement |
| **21+ conteneurs, complexe** | AKS | 238€+ | Orchestration nécessaire |
| **Jobs batch (5-10 min)** | ACI ou Functions | 2-5€ | Paie seulement le temps d'exécution |
| **Serveur web 24/7** | VM ou App Service | 15-50€ | Plus rentable que ACI |
| **Tâches événementielles** | Azure Functions | 0-10€ | Serverless, auto-scale |
| **Serveur de base de données** | VM + Disque Géré | 25-60€ | Besoin de persistence |
| **Apprentissage/Tests** | Local (k3s/minikube) | 0€ | Zéro coûts Azure ! |

---

## 🔍 Comparaison Détaillée

### 1️⃣ Azure Container Instances (ACI)

**C'est Quoi :**
- Conteneurs serverless
- Déploie des conteneurs individuels ou groupes
- Facturation à la seconde
- Aucune gestion de cluster

**Tarification :**
```
vCPU : 0,0000133€/seconde (~34€/mois pour 1 vCPU continu)
Mémoire : 0,0000015€/GB/seconde (~4€/mois pour 1GB continu)
Niveau gratuit : 50 000 vCPU-secondes/mois (~14 heures à 1 vCPU)
```

**✅ Idéal Pour :**
- 1-10 conteneurs
- Charges courtes (minutes à heures)
- Agents de build CI/CD
- Traitement batch
- Tests et développement
- Architectures simples

**❌ À Éviter Pour :**
- 20+ conteneurs (trop complexe à gérer)
- Besoin de politiques d'auto-scaling
- Besoin de service mesh (mTLS)
- Exigences réseau complexes
- Besoin de compétences Kubernetes

**Exemples de Coûts Réels :**
```
Ta secure-app (0,5 CPU, 0,5 GB) :
├─ 7 minutes de test : 0,00€ (dans le niveau gratuit) ✅
├─ 1 heure par jour : 0,50€/mois ✅
├─ 24/7 : 17€/mois ⚠️ (VM moins cher !)
```

### 2️⃣ Azure Kubernetes Service (AKS)

**C'est Quoi :**
- Cluster Kubernetes géré
- Orchestre des centaines de conteneurs
- Auto-scaling, mises à jour progressives, auto-guérison
- Fonctionnalités enterprise

**Tarification :**
```
Plan de contrôle : 0€ (Microsoft paie !)
Nœuds workers : 70€/mois chacun (D2s_v3 : 2 vCPU, 8GB RAM)
Load Balancer : 15€/mois
Stockage : 5-15€/mois
Minimum : 238€/mois (cluster à 3 nœuds)
```

**✅ Idéal Pour :**
- 20+ conteneurs
- Architecture microservices
- Besoin d'auto-scaling
- Déploiements sans interruption
- Exigences de service mesh
- Multi-environnements (dev/staging/prod)
- Apprendre Kubernetes pour le CV

**❌ À Éviter Pour :**
- 1-10 conteneurs (surdimensionné)
- Architectures simples
- Budget <200€/mois
- Pas besoin des fonctions d'orchestration
- Phase d'apprentissage (utilise K8s local à la place)

**Exemples de Coûts Réels :**
```
Petit AKS (3 nœuds) :
├─ Nœuds workers (3× D2s_v3) : 210€/mois
├─ Load Balancer : 15€/mois
├─ Stockage : 10€/mois
├─ Total : 235€/mois

Grand AKS (10 nœuds) :
├─ Nœuds workers (10× D2s_v3) : 700€/mois
├─ Load Balancer : 15€/mois
├─ Stockage : 30€/mois
├─ Total : 745€/mois
```

### 3️⃣ Machines Virtuelles (VMs)

**C'est Quoi :**
- Ordinateur virtuel complet
- Contrôle total de l'OS
- Stockage persistant
- Approche traditionnelle

**Tarification :**
```
B1s (1 vCPU, 1GB) : 7,50€/mois
B2s (2 vCPU, 4GB) : 30€/mois
D2s_v3 (2 vCPU, 8GB) : 70€/mois
+ Disque Géré (30GB SSD) : 4€/mois
+ IP Publique (statique) : 3€/mois
```

**✅ Idéal Pour :**
- Services 24/7 (serveurs web, APIs)
- Bases de données (besoin de persistence)
- Applications legacy
- Besoin de contrôle total de l'OS
- Installation logicielle complexe
- Coûts mensuels prévisibles

**❌ À Éviter Pour :**
- Charges courtes (paies 24/7 même inactif)
- Besoin de scaling fréquent
- Applications stateless
- Architectures container-first

**Exemples de Coûts Réels :**
```
VM unique (B1s + Disque + IP) :
├─ VM : 7,50€/mois
├─ Disque : 4€/mois
├─ IP : 3€/mois
├─ Total : 14,50€/mois ✅ Le moins cher pour 24/7 !

3 VMs (pour HA) :
├─ VMs : 22,50€/mois
├─ Disques : 12€/mois
├─ IPs : 9€/mois
├─ Load Balancer : 15€/mois
├─ Total : 58,50€/mois
```

### 4️⃣ Azure Functions (Serverless)

**C'est Quoi :**
- Exécution de code événementiel
- Aucun serveur à gérer
- Paie par exécution
- Scale infini automatique

**Tarification :**
```
Niveau gratuit : 1 million d'exécutions/mois + 400 000 GB-secondes
Après : 0,000014€/exécution + 0,000014€/GB-seconde
```

**✅ Idéal Pour :**
- Tâches événementielles (upload fichier → traitement)
- Backends API (endpoints simples)
- Tâches planifiées (cron jobs)
- Webhooks
- Traitement données IoT
- Vrais besoins serverless

**❌ À Éviter Pour :**
- Tâches longues (>10 minutes)
- Gestion d'état complexe
- Workflows basés conteneurs
- Apprentissage développement full-stack

**Exemples de Coûts Réels :**
```
Petite charge (10 000 exécutions/jour) :
├─ 300 000/mois exécutions
├─ Coût : 0,00€ (dans le niveau gratuit) ✅

Charge moyenne (100 000 exécutions/jour) :
├─ 3 000 000/mois exécutions
├─ Coût : ~28€/mois
```

### 5️⃣ Azure App Service (PaaS)

**C'est Quoi :**
- Plateforme pour applications web
- Aucune gestion d'infrastructure
- Scaling intégré, slots de déploiement
- Supporte Node.js, Python, .NET, PHP, Java

**Tarification :**
```
Niveau gratuit : 0€ (limité, pas de domaine personnalisé)
B1 (Basic) : 12€/mois
S1 (Standard) : 60€/mois
P1v2 (Premium) : 140€/mois
```

**✅ Idéal Pour :**
- Applications web sans conteneurs
- Applications .NET ou Java
- Besoin de slots de déploiement (blue/green)
- Besoins PaaS simples
- Intégration CI/CD

**❌ À Éviter Pour :**
- Architecture container-first (utilise ACI/AKS)
- Microservices complexes
- Besoin de Kubernetes
- Apprentissage DevOps moderne (conteneurs mieux)

---

## 🎯 Scénarios Réels

### Scénario 1 : Ta Secure-App (Conteneur Unique)

**Appli :** Serveur web Node.js (conteneur du Jour 35)

**Options :**

| Service | Coût/Mois | Avantages | Inconvénients | Verdict |
|---------|-----------|-----------|---------------|---------|
| **ACI** | 17€ (24/7) | Simple, pas de gestion | Pas de persistence | ✅ **MEILLEUR** |
| **VM B1s** | 15€ (24/7) | Disque persistant, contrôle total | Gérer les mises à jour OS | ✅ Aussi bien |
| **AKS** | 238€ | Surdimensionné pour 1 conteneur | Trop cher | ❌ Non |
| **Functions** | Ne peut pas héberger serveur web | - | Pas adapté | ❌ Non |
| **App Service** | 12€ (niveau F1 gratuit) | Déploiement facile | Moins de contrôle que conteneur | ⚠️ OK |

**GAGNANT : ACI** pour une appli conteneurisée unique, ou **App Service Gratuit** si pas de conteneurs.

---

### Scénario 2 : Startup avec 15 Microservices

**Appli :** Plateforme e-commerce
- 3 Services frontend (React)
- 8 APIs backend (Node.js)
- 2 Workers en arrière-plan
- 2 Bases de données (PostgreSQL, Redis)

**Options :**

| Service | Coût/Mois | Complexité | Scalabilité | Verdict |
|---------|-----------|------------|-------------|---------|
| **15 conteneurs ACI** | ~255€ | Élevée (gérer 15 individuellement) | Manuelle | ❌ Difficile à gérer |
| **AKS** | 238€ | Moyenne (courbe d'apprentissage K8s) | Automatique | ✅ **MEILLEUR** |
| **5 VMs + Docker** | ~75€ | Moyenne (Docker Compose) | Manuelle | ⚠️ Option budget |
| **App Service (15 apps)** | ~180€ | Faible | Manuelle | ⚠️ OK mais cher |

**GAGNANT : AKS** - Avec 15 conteneurs, l'orchestration se rentabilise en temps économisé et auto-scaling.

---

### Scénario 3 : Traitement Batch d'Images

**Appli :** Traiter des images uploadées
- 1 000 images par jour
- Chaque traitement prend 5 minutes
- S'exécute seulement quand images uploadées

**Options :**

| Service | Runtime/Jour | Coût/Mois | Verdict |
|---------|--------------|-----------|---------|
| **ACI** | 5 000 minutes | ~10€ (paie seulement l'usage) | ✅ **MEILLEUR** |
| **VM (toujours allumée)** | 24/7 | 15€ | ❌ Gaspillage (inactive 95% du temps) |
| **AKS** | - | 238€ | ❌ Énorme surdimensionnement |
| **Azure Functions** | 5 000 exécutions | ~5€ | ✅ Aussi excellent |

**GAGNANT : Azure Functions** ou **ACI** - Tous deux serverless, pay-per-use parfait ici.

---

### Scénario 4 : Pic de Trafic Black Friday

**Appli :** Site e-commerce
- Normal : 10 conteneurs (100 req/sec)
- Black Friday : Besoin 100 conteneurs (1 000 req/sec)
- Pic dure 4 heures

**Options :**

| Service | Coût (Normal) | Coût (Pic 4h) | Scaling | Verdict |
|---------|---------------|---------------|---------|---------|
| **ACI** | 170€/mois | +280€ pour 4h | Manuel (lent) | ❌ Difficile de scaler vite |
| **AKS** | 238€/mois | 238€ (auto-scale) | Automatique (rapide !) | ✅ **MEILLEUR** |
| **VMs** | Garder 100 VMs ? | 1 500€/mois | Manuel | ❌ Beaucoup trop cher |

**GAGNANT : AKS** - Le Horizontal Pod Autoscaler gère les pics de trafic automatiquement.

---

### Scénario 5 : Apprendre Kubernetes

**Objectif :** Apprendre K8s sans coûts Azure

**Options :**

| Service | Coût | Valeur Apprentissage | Verdict |
|---------|------|---------------------|---------|
| **AKS** | 238€/mois | Élevée (vrai Azure) | ❌ Trop cher |
| **k3s local** | 0€ | Élevée (95% identique à AKS) | ✅ **MEILLEUR** |
| **Minikube** | 0€ | Élevée (100% vrai K8s) | ✅ **MEILLEUR** |
| **Kind** | 0€ | Élevée (K8s dans Docker) | ✅ Aussi excellent |
| **AKS (test 1h)** | ~0,30€ | Élevée mais stressant | ⚠️ Risqué (peut oublier de supprimer) |

**GAGNANT : k3s ou minikube local** - Apprends K8s pour 0€, déploie sur AKS plus tard quand tu as un job ! 🎯

---

## 📈 Graphique de Comparaison Coûts

**Scénario : Exécuter 10 Conteneurs 24/7 pour 1 Mois**

```
Service         | Coût/Mois | Effort Gestion    | Auto-Scale | HA
----------------|-----------|-------------------|------------|----
ACI (10 × 0,5)  | 170€      | Élevé (10 manuel) | Non        | Manuel
AKS (3 nœuds)   | 238€      | Moyen (K8s)       | Oui        | Intégré
VMs (3 × B2s)   | 90€       | Élevé (SSH, MAJ)  | Non        | Manuel
Functions       | N/A       | Faible            | Oui        | Intégré
```

**Verdict :**
- Budget <100€ : **VMs avec Docker Compose**
- Budget 100-200€ : **ACI** si simple, **VMs** si besoin persistence
- Budget >200€ : **AKS** (l'orchestration vaut le coup)

---

## 🎯 Arbre de Décision

```
DÉBUT : J'ai besoin d'exécuter des conteneurs

├─ Combien de conteneurs ?
│  ├─ 1-5 conteneurs
│  │  ├─ Tourne 24/7 ?
│  │  │  ├─ OUI → ACI ou VM (coût similaire ~15-30€/mois)
│  │  │  └─ NON (courte durée) → ACI ou Functions (pay-per-use)
│  │  └─ Apprentissage seulement ? → Docker local (0€)
│  │
│  ├─ 6-20 conteneurs
│  │  ├─ Budget >200€/mois ?
│  │  │  ├─ OUI → AKS (vaut l'orchestration)
│  │  │  └─ NON → VMs + Docker Compose (50-100€/mois)
│  │  └─ Réseau/scaling complexe ?
│  │     ├─ OUI → AKS (besoin des fonctions K8s)
│  │     └─ NON → ACI avec Load Balancer
│  │
│  └─ 21+ conteneurs
│     ├─ Budget >200€/mois ?
│     │  ├─ OUI → AKS (obligatoire à cette échelle)
│     │  └─ NON → Multi-VM Docker Swarm (100-150€/mois)
│     └─ Apprentissage ? → k3s local (0€)

├─ Besoin compétences Kubernetes ?
│  ├─ OUI → k3s/minikube local (apprentissage gratuit)
│  └─ NON → Utilise options plus simples (ACI, App Service)

├─ Besoin d'auto-scaling ?
│  ├─ OUI → AKS ou Functions
│  └─ NON → ACI ou VMs

├─ Besoin de mises à jour sans interruption ?
│  ├─ OUI → AKS (rolling updates) ou Functions
│  └─ NON → ACI ou VMs (blue/green manuel)

└─ Contrainte budgétaire ?
   ├─ 0-20€/mois → Local + Functions (niveau gratuit)
   ├─ 20-100€/mois → ACI ou VMs
   ├─ 100-200€/mois → ACI + LB ou VMs
   └─ 200€+/mois → AKS (orchestration complète)
```

---

## 💡 Conseils Pro

### Quand ACI est Parfait :
✅ Ton conteneur du Jour 35 (appli unique)  
✅ Agents de build CI/CD (tourne 10 minutes, détruit)  
✅ Traitement batch (lance job, éteint)  
✅ Tester avant de s'engager sur AKS  
✅ Projets perso avec peu de trafic

### Quand AKS Vaut le Coup :
✅ 20+ microservices  
✅ Besoin de politiques d'auto-scaling  
✅ Déploiements sans interruption critiques  
✅ Service mesh (mTLS entre services)  
✅ Construire compétences Kubernetes pour CV  
✅ Charges production enterprise

### Quand les VMs Sont Meilleures :
✅ Bases de données (besoin stockage persistant)  
✅ Applications legacy (impossible à conteneuriser)  
✅ Services simples 24/7 (moins cher que ACI)  
✅ Besoin contrôle total OS  
✅ Budget mensuel prévisible

### Quand Functions Gagnent :
✅ Événementiel (upload fichier déclenche traitement)  
✅ Webhooks (reçoit HTTP, traite, répond)  
✅ Tâches planifiées (cron jobs)  
✅ Vrais besoins serverless  
✅ Budget = 0€ (niveau gratuit généreux)

---

## 🔢 Calculateur de Coûts

**Ta Charge de Travail :**
- Conteneurs : _____ (nombre)
- Runtime : _____% (pourcentage du mois en exécution)
- CPU par conteneur : _____
- Mémoire par conteneur : _____

**Coûts Estimés :**

```
Coût ACI :
= (Conteneurs × CPU × 0,0000133 × 2 592 000 × Runtime%) 
+ (Conteneurs × Mémoire × 0,0000015 × 2 592 000 × Runtime%)

Exemple : 5 conteneurs, 0,5 CPU, 0,5 GB, 50% runtime
= (5 × 0,5 × 0,0000133 × 2 592 000 × 0,5)
+ (5 × 0,5 × 0,0000015 × 2 592 000 × 0,5)
= 43€ + 4,86€
= 48€/mois

Coût AKS (3 nœuds) :
= 238€/mois (fixe, peu importe le nombre de conteneurs jusqu'à capacité des nœuds)

Coût VM :
= (VMs × prix_VM) + (Disques × 4€) + (IPs × 3€)

Exemple : 3 VMs (B1s), 3 disques, 1 IP publique
= (3 × 7,50€) + (3 × 4€) + (1 × 3€)
= 22,50€ + 12€ + 3€
= 37,50€/mois
```

---

## 🎓 Pour Ton Portfolio/Entretiens

**Montre Que Tu Comprends les Compromis :**

> "Pour mon projet secure-app, j'ai déployé sur Azure Container Instances car c'est un conteneur unique tournant 24/7. ACI coûte 17€/mois ce qui est comparable à une VM, mais plus simple à gérer. J'ai considéré AKS mais à 238€/mois minimum, c'est surdimensionné pour un conteneur. Si j'exécutais 20+ microservices, AKS aurait du sens pour l'auto-scaling et l'orchestration. Pour apprendre Kubernetes, j'ai pratiqué localement avec k3s pour éviter les coûts."

**Cela montre :**
- ✅ Tu comprends les implications de coût
- ✅ Tu prends des décisions architecturales basées sur les besoins
- ✅ Tu sais quand NE PAS sur-ingénierer
- ✅ Tu es conscient du budget (précieux dans toute entreprise !)

---

## 📚 Guides Connexes

- [Jour 39 : Azure Container Instances](../02-security-services/day39-container-instances/README.md)
- [Jour 41 : Théorie AKS & Kubernetes](../02-security-services/day41-aks-kubernetes-theory/README.md)
- [COST-REALITY.md](../02-security-services/day39-container-instances/COST-REALITY.md)
- [ACRONYM-DECODER.md](./ACRONYM-DECODER.md)
- [Version anglaise : DECISION-MATRIX-EN.md](./DECISION-MATRIX-EN.md)

---

**Créé :** 25 octobre 2025  
**Auteur :** YomHub  
**Objectif :** Prendre des décisions architecturales cloud intelligentes  
**Langue :** Français (Version anglaise : DECISION-MATRIX-EN.md)  
**Usage :** Planification architecture, préparation entretiens, optimisation coûts

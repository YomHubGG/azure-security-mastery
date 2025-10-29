# Day 43: Local Kubernetes Practice - kubectl Commands

## 🎯 Essential kubectl Commands (Practice These!)

### View Resources
```bash
# List all namespaces
sudo kubectl get namespaces

# List pods in secure-app namespace
sudo kubectl get pods -n secure-app

# List all resources
sudo kubectl get all -n secure-app

# Watch pods in real-time
sudo kubectl get pods -n secure-app -w

# See detailed pod info
sudo kubectl describe pod <pod-name> -n secure-app
```

### Logs & Debugging
```bash
# View logs from a pod
sudo kubectl logs <pod-name> -n secure-app

# Follow logs in real-time
sudo kubectl logs -f <pod-name> -n secure-app

# Get shell inside running container
sudo kubectl exec -it <pod-name> -n secure-app -- sh

# Check pod resource usage
sudo kubectl top pods -n secure-app
```

### Test High Availability (Delete a Pod!)
```bash
# Delete one pod (Kubernetes will auto-recreate it!)
sudo kubectl delete pod <pod-name> -n secure-app

# Watch it respawn automatically
sudo kubectl get pods -n secure-app -w
```

### Scaling
```bash
# Scale to 5 replicas
sudo kubectl scale deployment secure-app -n secure-app --replicas=5

# Scale back to 3
sudo kubectl scale deployment secure-app -n secure-app --replicas=3
```

### Update & Rollback
```bash
# Update image (rolling update)
sudo kubectl set image deployment/secure-app secure-app=ghcr.io/yomhubgg/secure-app:2.0.0 -n secure-app

# Check rollout status
sudo kubectl rollout status deployment/secure-app -n secure-app

# Rollback to previous version
sudo kubectl rollout undo deployment/secure-app -n secure-app
```

### Cleanup
```bash
# Delete everything in namespace
sudo kubectl delete namespace secure-app

# Or delete individual resources
sudo kubectl delete deployment secure-app -n secure-app
sudo kubectl delete service secure-app-service -n secure-app
```

---

## 🧪 **EXERCISES: Try These Now!**

### Exercise 1: Pod Self-Healing
1. List your 3 pods: `sudo kubectl get pods -n secure-app`
2. Copy one pod name
3. Delete it: `sudo kubectl delete pod <pod-name> -n secure-app`
4. Immediately watch: `sudo kubectl get pods -n secure-app -w`
5. **Result**: Kubernetes auto-creates a new pod! That's self-healing!

### Exercise 2: Explore Inside a Pod
1. Get a pod name: `sudo kubectl get pods -n secure-app`
2. Exec into it: `sudo kubectl exec -it <pod-name> -n secure-app -- sh`
3. Check running processes: `ps aux`
4. Check user: `whoami` (should be "node" - not root!)
5. Check network: `netstat -tn`
6. Exit: `exit`

### Exercise 3: View Logs
1. Get pod name: `sudo kubectl get pods -n secure-app`
2. View logs: `sudo kubectl logs <pod-name> -n secure-app`
3. Follow live: `sudo kubectl logs -f <pod-name> -n secure-app`
4. In another terminal, curl the app: `curl http://localhost:30080`
5. See the request appear in logs!
6. Ctrl+C to stop

### Exercise 4: Scale Up/Down
1. Check current pods: `sudo kubectl get pods -n secure-app`
2. Scale to 5: `sudo kubectl scale deployment secure-app -n secure-app --replicas=5`
3. Watch it happen: `sudo kubectl get pods -n secure-app -w`
4. See 5 pods running!
5. Scale back to 2: `sudo kubectl scale deployment secure-app -n secure-app --replicas=2`
6. Watch 3 pods terminate
7. Scale back to 3: `sudo kubectl scale deployment secure-app -n secure-app --replicas=3`

### Exercise 5: High Availability Test
1. In terminal 1: `watch -n 1 curl -s http://localhost:30080 | head -1`
2. In terminal 2: Delete a pod while terminal 1 is running
3. **Result**: App keeps responding! Traffic automatically routed to other pods!

---

## 📊 What You're Learning

### Kubernetes Core Concepts
- ✅ **Pods**: Smallest deployable unit (1+ containers)
- ✅ **Deployments**: Manage desired state (3 replicas = always 3 pods)
- ✅ **Services**: Network exposure (load balancing)
- ✅ **Namespaces**: Isolation (separate environments)
- ✅ **ReplicaSets**: Ensure pod count (auto-created by Deployment)

### Kubernetes Benefits
- ✅ **Self-Healing**: Pod dies → K8s recreates it
- ✅ **Scaling**: Change replicas → K8s adds/removes pods
- ✅ **Load Balancing**: Service distributes traffic across pods
- ✅ **Rolling Updates**: Update without downtime
- ✅ **Declarative**: Describe desired state, K8s makes it happen

---

## 🎓 Compare to Day 39 (ACI)

| Feature | ACI (Day 39) | Kubernetes (Day 43) |
|---------|--------------|---------------------|
| **Deploy** | `az container create` | `kubectl apply -f deployment.yaml` |
| **Replicas** | Manual (create multiple containers) | Automatic (replicas: 3) |
| **Self-Healing** | No (dead = dead) | Yes (auto-recreates) |
| **Load Balancing** | Need Azure Load Balancer (+€15/mo) | Built-in (Service) |
| **Scaling** | Manual | One command (`kubectl scale`) |
| **Health Checks** | Manual monitoring | Built-in (liveness/readiness probes) |
| **Cost (Azure)** | €17/month per container | €238/month (AKS) |
| **Cost (Local)** | N/A | **€0** ✅ |

---

## 🚀 Next Steps

### Today (after exercises):
1. ✅ Create Network Policy (block internet access)
2. ✅ Implement Pod Security Standards
3. ✅ Test resource limits

### Tomorrow:
1. 🔄 Add ConfigMaps and Secrets
2. 🔄 Multi-container pods (sidecar pattern)
3. 🔄 Deploy to multiple namespaces (dev/staging/prod)

### Week After:
1. ❌ Skip AKS deployment (save €238/month)
2. ❌ Move to DevSecOps deep dive (Day 45+)
3. ❌ CI/CD with GitHub Actions deploying to local K8s

---

**Created**: October 29, 2025  
**Session**: #22  
**Day**: 43/365  
**Cost**: €0 (local K8s!)  
**Status**: 🎉 **YOU JUST MASTERED KUBERNETES BASICS!**

# Day 1 - Azure Setup

**Date**: Aug 27, 2025

## Issue
Portal wouldn't load on this machine. Had to use CLI instead.

## What I deployed
- Resource group: `rg-security-learning-001`
- Storage account: `stseclearn73696`
- Security: HTTPS-only, TLS 1.2, no public access, network deny

## Commands used
```bash
az login --use-device-code
az group create --name "rg-security-learning-001" --location "West Europe"
az storage account create --name "stseclearn73696" --https-only true --min-tls-version TLS1_2 --allow-blob-public-access false --default-action Deny
```

## Notes
CLI is actually faster than portal once you know the commands.

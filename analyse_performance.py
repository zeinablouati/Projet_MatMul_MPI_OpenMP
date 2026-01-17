import matplotlib.pyplot as plt
import numpy as np

# Données de benchmark (Matrice 2000x2000)
configurations = [
    'Séquentiel',
    'MPI 1p',
    'MPI 2p',
    'MPI 4p',
    'Hybrid\n2×2',
    'Hybrid\n2×4',
    'Hybrid\n4×2'
]

temps_ms = [4.322, 4.336, 2.545, 2.230, 4.102, 2.785, 3.547]
gflops = [1.851, 1.845, 3.144, 3.587, 1.950, 2.873, 2.256]

# Calcul du speedup par rapport au séquentiel
speedup = [temps_ms[0] / t for t in temps_ms]

# Nombre de workers pour chaque configuration
workers = [1, 1, 2, 4, 4, 8, 8]

# Efficacité parallèle (speedup / nombre de workers)
efficacite = [(s / w) * 100 for s, w in zip(speedup, workers)]

# Créer une figure avec 4 sous-graphiques
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Analyse de Performance - Multiplication Matrice-Vecteur (2000×2000)', 
             fontsize=16, fontweight='bold')

# Couleurs pour les différentes catégories
colors_cat = ['#2E86AB', '#A23B72', '#F18F01', '#C73E1D', '#06A77D', '#06A77D', '#06A77D']

# ========== Graphique 1: Temps d'exécution ==========
ax1.bar(range(len(configurations)), temps_ms, color=colors_cat, alpha=0.8, edgecolor='black')
ax1.set_xlabel('Configuration', fontweight='bold')
ax1.set_ylabel('Temps (ms)', fontweight='bold')
ax1.set_title('Temps d\'exécution', fontweight='bold')
ax1.set_xticks(range(len(configurations)))
ax1.set_xticklabels(configurations, rotation=45, ha='right', fontsize=9)
ax1.grid(axis='y', alpha=0.3, linestyle='--')
ax1.axhline(y=temps_ms[0], color='red', linestyle='--', linewidth=1, alpha=0.5, label='Baseline séquentiel')

# Ajouter les valeurs sur les barres
for i, v in enumerate(temps_ms):
    ax1.text(i, v + 0.1, f'{v:.2f}', ha='center', va='bottom', fontsize=8, fontweight='bold')

ax1.legend()

# ========== Graphique 2: Performance (GFLOPS) ==========
ax2.bar(range(len(configurations)), gflops, color=colors_cat, alpha=0.8, edgecolor='black')
ax2.set_xlabel('Configuration', fontweight='bold')
ax2.set_ylabel('GFLOPS', fontweight='bold')
ax2.set_title('Performance (GFLOPS)', fontweight='bold')
ax2.set_xticks(range(len(configurations)))
ax2.set_xticklabels(configurations, rotation=45, ha='right', fontsize=9)
ax2.grid(axis='y', alpha=0.3, linestyle='--')
ax2.axhline(y=gflops[0], color='red', linestyle='--', linewidth=1, alpha=0.5, label='Baseline séquentiel')

# Ajouter les valeurs sur les barres
for i, v in enumerate(gflops):
    ax2.text(i, v + 0.05, f'{v:.2f}', ha='center', va='bottom', fontsize=8, fontweight='bold')

ax2.legend()

# ========== Graphique 3: Speedup ==========
ax3.bar(range(len(configurations)), speedup, color=colors_cat, alpha=0.8, edgecolor='black')
ax3.set_xlabel('Configuration', fontweight='bold')
ax3.set_ylabel('Speedup', fontweight='bold')
ax3.set_title('Speedup vs Séquentiel', fontweight='bold')
ax3.set_xticks(range(len(configurations)))
ax3.set_xticklabels(configurations, rotation=45, ha='right', fontsize=9)
ax3.grid(axis='y', alpha=0.3, linestyle='--')

# Ligne de speedup idéal
ideal_speedup = [1, 1, 2, 4, 4, 8, 8]
ax3.plot(range(len(configurations)), ideal_speedup, 'r--', linewidth=2, 
         marker='o', markersize=4, label='Speedup idéal', alpha=0.6)

# Ajouter les valeurs sur les barres
for i, v in enumerate(speedup):
    ax3.text(i, v + 0.05, f'{v:.2f}x', ha='center', va='bottom', fontsize=8, fontweight='bold')

ax3.legend()
ax3.set_ylim(0, max(ideal_speedup) + 1)

# ========== Graphique 4: Efficacité parallèle ==========
ax4.bar(range(len(configurations)), efficacite, color=colors_cat, alpha=0.8, edgecolor='black')
ax4.set_xlabel('Configuration', fontweight='bold')
ax4.set_ylabel('Efficacité (%)', fontweight='bold')
ax4.set_title('Efficacité Parallèle', fontweight='bold')
ax4.set_xticks(range(len(configurations)))
ax4.set_xticklabels(configurations, rotation=45, ha='right', fontsize=9)
ax4.grid(axis='y', alpha=0.3, linestyle='--')
ax4.axhline(y=100, color='green', linestyle='--', linewidth=1, alpha=0.5, label='Efficacité idéale (100%)')
ax4.axhline(y=50, color='orange', linestyle=':', linewidth=1, alpha=0.5, label='Seuil acceptable (50%)')

# Ajouter les valeurs sur les barres
for i, v in enumerate(efficacite):
    ax4.text(i, v + 2, f'{v:.1f}%', ha='center', va='bottom', fontsize=8, fontweight='bold')

ax4.legend()
ax4.set_ylim(0, 120)

# Ajuster l'espacement
plt.tight_layout()

# Sauvegarder le graphique
plt.savefig('performance_analysis.png', dpi=300, bbox_inches='tight')
print("✓ Graphique sauvegardé: performance_analysis.png")

# Afficher le graphique
plt.show()

# ========== Créer un second graphique: Comparaison par taille de matrice ==========
print("\nGénération du graphique de scalabilité...")

# Données simulées pour différentes tailles (tu peux les remplacer par tes vrais résultats)
matrix_sizes = [500, 1000, 2000, 5000]

# Temps pour chaque configuration (en ms) - REMPLACE PAR TES VRAIES DONNÉES
sequential_times = [0.282, 0.936, 4.322, 45.0]  # Estimation
mpi_2_times = [0.150, 0.567, 2.545, 25.0]       # Estimation
mpi_4_times = [0.100, 0.490, 2.230, 20.0]       # Estimation

fig2, ax = plt.subplots(figsize=(10, 6))

ax.plot(matrix_sizes, sequential_times, 'o-', linewidth=2, markersize=8, label='Séquentiel', color='#2E86AB')
ax.plot(matrix_sizes, mpi_2_times, 's-', linewidth=2, markersize=8, label='MPI 2 processus', color='#F18F01')
ax.plot(matrix_sizes, mpi_4_times, '^-', linewidth=2, markersize=8, label='MPI 4 processus', color='#C73E1D')

ax.set_xlabel('Taille de la matrice (N×N)', fontweight='bold', fontsize=12)
ax.set_ylabel('Temps d\'exécution (ms)', fontweight='bold', fontsize=12)
ax.set_title('Scalabilité: Temps d\'exécution vs Taille de matrice', fontweight='bold', fontsize=14)
ax.set_xscale('log')
ax.set_yscale('log')
ax.grid(True, alpha=0.3, linestyle='--')
ax.legend(fontsize=10, loc='upper left')

# Annoter les points
for i, size in enumerate(matrix_sizes):
    if i % 2 == 0:  # Annoter un point sur deux pour éviter le chevauchement
        ax.annotate(f'{size}×{size}', 
                   xy=(size, sequential_times[i]), 
                   xytext=(10, 10), 
                   textcoords='offset points',
                   fontsize=8, alpha=0.7)

plt.tight_layout()
plt.savefig('scalability_analysis.png', dpi=300, bbox_inches='tight')
print("✓ Graphique sauvegardé: scalability_analysis.png")

plt.show()

# ========== Tableau récapitulatif dans le terminal ==========
print("\n" + "="*80)
print("TABLEAU RÉCAPITULATIF - Performance Matrice 2000×2000")
print("="*80)
print(f"{'Configuration':<15} {'Temps (ms)':<12} {'GFLOPS':<10} {'Speedup':<10} {'Efficacité':<12}")
print("-"*80)

for i, config in enumerate(configurations):
    config_clean = config.replace('\n', ' ')
    print(f"{config_clean:<15} {temps_ms[i]:<12.3f} {gflops[i]:<10.3f} {speedup[i]:<10.2f} {efficacite[i]:<12.1f}%")

print("="*80)

# Meilleure configuration
best_idx = speedup.index(max(speedup))
print(f"\n🏆 Meilleure configuration: {configurations[best_idx].replace(chr(10), ' ')}")
print(f"   Speedup: {speedup[best_idx]:.2f}x")
print(f"   Performance: {gflops[best_idx]:.2f} GFLOPS")
print(f"   Efficacité: {efficacite[best_idx]:.1f}%")
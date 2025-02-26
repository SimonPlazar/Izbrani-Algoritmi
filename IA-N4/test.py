from main import SIMPLEX
import numpy as np
import time
import matplotlib.pyplot as plt


def TEST_SIMPLEX(max_size, num_repeats):
    results = []
    for n in range(2, max_size + 1):
        times = []
        for _ in range(num_repeats):
            A = np.abs(np.random.uniform(0, 1000, (2 * n, 2 * n)))
            b = np.abs(np.random.uniform(0, 1000, 2 * n))
            c = np.abs(np.random.uniform(0, 1000, 2 * n))

            start_time = time.time()
            SIMPLEX(A, b, c, n, n)
            times.append((time.time() - start_time) * 1000)

        results.append({
            "size": n,
            "max_time": max(times),
            # "min_time": min([cur for cur in times if cur > 0]),
            "min_time": min(times),
            "avg_time": sum(times) / len(times)
        })

    return results


if __name__ == "__main__":
    results = TEST_SIMPLEX(25, 100)
    for result in results:
        print(result)

    # Ekstrahiranje podatkov za graf
    sizes = [entry['size'] for entry in results]
    max_times = [entry['max_time'] for entry in results]
    min_times = [entry['min_time'] for entry in results]
    avg_times = [entry['avg_time'] for entry in results]

    # Risanje grafa
    plt.figure(figsize=(10, 6))
    plt.plot(sizes, max_times, label='Max Time (ms)', marker='o')
    plt.plot(sizes, min_times, label='Min Time (ms)', marker='o')
    plt.plot(sizes, avg_times, label='Avg Time (ms)', marker='o')
    plt.xlabel('Size of Matrix A')
    plt.ylabel('Time (ms)')
    plt.title('Performance of Simplex Algorithm')
    plt.legend()
    plt.grid(True)
    plt.savefig('simplex_performance.png')

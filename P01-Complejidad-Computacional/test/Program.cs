using DAA.Base;
using DAA.Implementaciones;

class Program {
    static void Main() {
        int[] tamaños = { 2, 4, 8, 10, 25, 50, 100, 250, 500, 750, 1000 };
        Algoritmo[] variantes = { new ProductoPorFilas(), new ProductoPorColumnas() };

        Console.WriteLine("N\t\tAlgoritmo\t\tTiempo (ms)");
        Console.WriteLine("--------------------------------------------------");

        foreach (int n in tamaños) {
            var datos = FabricaInstancias.CrearAleatoria(n);
            foreach (var variante in variantes) {
                Solucion sol = variante.Resolver(datos);
                Console.WriteLine($"{n}\t\t{variante.GetType().Name}\t{sol.ValorObjetivo:F4}");
            }
            Console.WriteLine("--------------------------------------------------");
        }
    }
}
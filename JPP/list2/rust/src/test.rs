mod ring;

fn test_ring<const N: u64>(a: i64, b: i64) {
    let x = ring::Ring::<N>::new(a);
    let y = ring::Ring::<N>::new(b);

    println!("Testing with base {}, x = {}, y = {}:", N, a, b);
    println!("x:     {}", x);
    println!("y:     {}", y);
    println!("x + y: {}", x + y);
    println!("x - y: {}", x - y);
    println!("-x:    {}", -x);
    println!("x * y: {}", x * y);

    match x / y {
        Ok(result) => println!("x / y: {}", result),
        Err(e) => println!("x / y: Error during division: {}", e),
    }
    println!();
}

fn main() {
    test_ring::<10>(3, 7);
    test_ring::<5>(2, 4);
    test_ring::<71>(36, 45);
    test_ring::<23>(13, 4);
    test_ring::<12>(5, 0);
    test_ring::<8>(5, 4);
}
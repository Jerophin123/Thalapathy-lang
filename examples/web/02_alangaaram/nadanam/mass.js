document.addEventListener("DOMContentLoaded", () => {
    console.log("Vanakkam Nanba! Nadanam browser side-la aarambichiduchu. 🔥");
    const cta = document.querySelector(".cta");
    if (cta) {
        cta.addEventListener("mouseenter", () => { cta.style.transform = "scale(1.05)"; });
        cta.addEventListener("mouseleave", () => { cta.style.transform = "scale(1)"; });
    }
});

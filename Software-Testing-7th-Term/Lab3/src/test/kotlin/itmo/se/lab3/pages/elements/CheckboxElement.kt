package itmo.se.lab3.pages.elements

import org.openqa.selenium.*

class CheckboxElement(container: WebElement) {
    private val label = container.findElement(By.xpath(".//label"))
    private val input = container.findElement(By.xpath(".//input[@type=\"checkbox\"]"))

    val text: String = label.getAttribute("textContent")

    val checked get() = input.isSelected

    fun toggle() = label.click()
}

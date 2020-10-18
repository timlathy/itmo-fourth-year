package itmo.se.lab3.pages.elements

import org.openqa.selenium.*
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.WebDriverWait

class ListingElement(private val driver: WebDriver, private val container: WebElement) {
    companion object {
        fun findAll(driver: WebDriver): List<ListingElement> =
            driver.findElements(By.xpath("//div[starts-with(@class, \"shop_row \")]"))
                .map { ListingElement(driver, it) }
    }

    val title: String = container.findElement(By.xpath(".//a[@class=\"order-title\"]")).text
    val description: String = container.findElement(By.xpath(".//div[@class=\"job_desc\"]")).text
    val price = container.findElement(By.xpath("//div[@class=\"price\"]")).text.removeSuffix(" руб.").toDouble()
    val keywords =
        container.findElement(By.xpath("//div[starts-with(@id, \"tab-keywords\")]")).text.split(Regex(" \\d+, "))

    private val addToCartButton: WebElement? = container.findElement(By.xpath("//a[text()=\"Отложить в корзину\"]"))
    private val removeFromCartButton: WebElement? =
        container.findElement(By.xpath(".//a[text()=\"Удалить из корзины\"]"))

    fun addToCart() {
        addToCartButton!!.click()
        val statusLocator = By.xpath(".//*[contains(text(), \"добавлена в\")]")
        WebDriverWait(driver, 2).until(presenceOfNestedElementLocatedBy(container, statusLocator))
    }

    fun removeFromCart() {
        removeFromCartButton!!.click()
        val statusLocator = By.xpath(".//*[contains(text(), \"удалена из корзины\")]")
        WebDriverWait(driver, 2).until(presenceOfNestedElementLocatedBy(container, statusLocator))
    }
}
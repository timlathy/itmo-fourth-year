package itmo.se.lab3.pages

import itmo.se.lab3.pages.elements.ListingElement
import org.openqa.selenium.*
import org.openqa.selenium.support.PageFactory

class MarketplaceCartPage(private val driver: WebDriver) {
    init {
        driver["https://advego.com/shop/my/cart/"]
        PageFactory.initElements(driver, this)
    }

    fun getTotalIncludingFee(): Double =
        driver.findElement(By.xpath("//h2[starts-with(text(), \"Корзина /\")]")).text.let {
            Regex("Корзина / ([.\\d]+)").find(
                it
            )!!.groups[1]!!.value.toDouble()
        }

    fun listItems(): List<ListingElement> = ListingElement.findAll(driver)
}
